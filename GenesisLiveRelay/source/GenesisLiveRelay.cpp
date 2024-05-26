#include "GenesisLiveRelay/GenesisLiveRelay.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshLogger/AshLoggerPassage.h"
#include "AshLogger/AshLoggerTag.h"
#include "GenesisLiveRelay/GenesisLiveRelayPeerConnection.hpp"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConfig.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <fstream>
#include <stack>
#include <string>
#include <thread>
#include <utility>

namespace genesis::live
{

    using namespace std::chrono_literals;

    GenesisLiveRelay::GenesisLiveRelay() : m_AlwaysOpenConnection(nullptr), m_Logger("Relay", {}), m_PeerCounter(), m_ConnectedPeers(), m_TimeoutDuration(GenesisLiveRelayConfig::smTimeoutDuration)
    {
        m_Logger.AddLoggerPassage(
            new ash::AshLoggerFunctionPassage([](ash::AshLoggerDefaultPassage* This, ash::AshLoggerTag Tag, std::string Format, fmt::format_args Args, std::string FormattedString) -> void
                                              { std::cout << "[" << This->GetParent()->GetLoggerName() << "::" << Tag.GetPrefix() << "] " << FormattedString << std::endl; }));
    }

    ash::AshResult GenesisLiveRelay::AllocateNewAlwaysOpenConnection()
    {
        m_AlwaysOpenConnection = new GenesisLiveRelayPeerConnection();

        m_AlwaysOpenConnection->SetGatheredServerReflexiveCandidateCallback(
            [this](GenesisLiveRelayConnection* Connection) -> void
            {
                m_Logger.Log("Info", "First Invite Code saved into InviteCode.txt");

                std::ofstream outputStream = std::ofstream("InviteCode.txt", std::ios::trunc);
                outputStream << Connection->GetMyConnectionString();
                outputStream.close();
            });

        m_AlwaysOpenConnection->SetConnectedToRemoteCallback(
            [this](GenesisLiveRelayConnection* Connection) -> void
            {
                m_Logger.Log("Info", "Client connected: {}", Connection->GetConnection()->remoteAddress()->data());

                this->MoveAlwaysOpenConnectionToNextStage();
            });

        m_AlwaysOpenConnection->SetDisconnectedFromRemoteCallback([this](GenesisLiveRelayConnection* ConnectionUnCast) -> void
                                                                  { reinterpret_cast<GenesisLiveRelayPeerConnection*>(ConnectionUnCast)->ForceTimeout(); });

        m_AlwaysOpenConnection->SetMessageCallback(
            [this](GenesisLiveRelayConnection* ConnectionUnCast, ash::AshBuffer* Buffer) -> void
            {
                GenesisLiveRelayPacketBase* packetDeserialized = nullptr;

                if (auto res = GenesisLiveRelayPacketUtils::sfDeserializePacket("GenesisRelayPacket", Buffer); res.HasError())
                {
                    GenesisLiveRelayPeerConnection* connection = reinterpret_cast<GenesisLiveRelayPeerConnection*>(ConnectionUnCast);
                    connection->ForceTimeout();

                    m_Logger.Log("Error", "Kicking {} because failed to deserialize packet. {}", connection->GetPeerId(), res.GetMessage());

                    return;
                }
                else
                {
                    packetDeserialized = res.GetResult();
                }

                this->HandleMessage(reinterpret_cast<GenesisLiveRelayPeerConnection*>(ConnectionUnCast), packetDeserialized);

                delete packetDeserialized;
            });

        return m_AlwaysOpenConnection->InitializeConnection();
    }

    ash::AshResult GenesisLiveRelay::MoveAlwaysOpenConnectionToNextStage()
    {
        GenesisLiveRelayPeerConnection* connection = m_AlwaysOpenConnection;
        m_AlwaysOpenConnection = nullptr;

        if (auto res = AllocateNewAlwaysOpenConnection(); res.HasError())
        {
            return res;
        }

        m_PeerCounter++;
        connection->SetPeerId(m_PeerCounter);
        connection->TouchPing();

        m_ConnectedPeers.emplace(connection->GetPeerId(), connection);

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLiveRelay::RemovePeer(GenesisPeerId PeerId)
    {
        if (m_ConnectedPeers.contains(PeerId) == false)
        {
            return ash::AshResult(false, "Unknown peer");
        }

        GenesisLiveRelayPeerConnection* peer = m_ConnectedPeers.at(PeerId);
        m_ConnectedPeers.erase(PeerId);

        delete peer;

        // Add announcing to everybody here
        // @TODO

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLiveRelay::HandleMessage(GenesisLiveRelayPeerConnection* Connection, GenesisLiveRelayPacketBase* Packet)
    {
        switch (Packet->GetType())
        {
        case GenesisLiveRelayPacketType::CLIENT_CONNECT_REQUEST:
        {
            GenesisLiveRelayPacketClientConnectRequest* connectRequest = reinterpret_cast<GenesisLiveRelayPacketClientConnectRequest*>(Packet);
            GenesisLiveRelayPacketClientConnectResponse connectResponse = GenesisLiveRelayPacketClientConnectResponse();

            m_Logger.Log("Info", "Received connect request with username: {}", connectRequest->GetClientName());

            connectResponse.SetReason(GenesisLiveRelayPacketClientConnectResponse::ReasonType::FORCE_ASSIGN_TO_YOU);
            connectResponse.SetAssignedPeerId(Connection->GetPeerId());
            connectResponse.SetAssignedClientName(fmt::format("{} #{}", connectRequest->GetClientName(), Connection->GetPeerId()));

            Connection->SetState(GenesisLiveRelayPeerConnection::StateType::AUTHED);

            Connection->SendPacket(&connectResponse);

            // Broadcast to others

            connectResponse.SetReason(GenesisLiveRelayPacketClientConnectResponse::ReasonType::CLIENT_JOINED);

            BroadcastPacketsToPeers(&connectResponse, {Connection->GetPeerId()});

            break;
        }
        case GenesisLiveRelayPacketType::PING:
        {
            Connection->TouchPing();
            break;
        }
        case GenesisLiveRelayPacketType::FORWARD_TO:
        default:
            break;
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLiveRelay::ForeachPeer(GenesisLiveRelay::sdForeachPeerDelegate Function, std::vector<GenesisPeerId> AvoidPeers)
    {
        for (auto currentIterator : m_ConnectedPeers)
        {
            if (std::find(AvoidPeers.begin(), AvoidPeers.end(), currentIterator.first) == AvoidPeers.end())
            {
                Function(currentIterator.first, currentIterator.second);
            }
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLiveRelay::BroadcastPacketsToPeers(GenesisLiveRelayPacketBase* Packet, std::vector<GenesisPeerId> AvoidPeers)
    {
        return ForeachPeer([Packet](GenesisPeerId PeerId, GenesisLiveRelayPeerConnection* Peer) -> void { Peer->SendPacket(Packet); }, AvoidPeers);
    }

    void GenesisLiveRelay::Run()
    {
        AllocateNewAlwaysOpenConnection();

        std::string connectionString = "";

        m_Logger.Log("Info", "Please paste in your invite code: ");
        std::cin >> connectionString;

        m_AlwaysOpenConnection->SetRemoteConnectionString(connectionString);

        while (1)
        {
            std::stack<GenesisPeerId> forceRemovalOfPeers = {};

            for (auto currentIterator : m_ConnectedPeers)
            {
                if (currentIterator.second->HasTimeouted(m_TimeoutDuration))
                {
                    forceRemovalOfPeers.push(currentIterator.first);
                    m_Logger.Log("Error", "Removing peer {} because of timeout.", currentIterator.first);
                }
            }

            while (forceRemovalOfPeers.empty() == false)
            {
                GenesisPeerId currentRemovalPeerId = forceRemovalOfPeers.top();
                forceRemovalOfPeers.pop();

                this->RemovePeer(currentRemovalPeerId);
            }

            std::this_thread::sleep_for(m_TimeoutDuration * 2.5);
            std::this_thread::yield();
        }
    }

} // namespace genesis::live