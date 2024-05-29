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

    GenesisLiveRelay::GenesisLiveRelay()
        : m_AlwaysOpenConnection(nullptr), m_Logger("Relay", {}), m_PeerCounter(), m_ConnectedPeers(), m_TimeoutDuration(GenesisLiveRelayConfig::smTimeoutDuration), m_HasFirstPeerBeenConnected(false)
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
                if (m_ConnectedPeers.size() <= 0)
                {
                    m_Logger.Log("Info", "First Invite Code saved into InviteCode.txt");

                    std::ofstream outputStream = std::ofstream("InviteCode.txt", std::ios::trunc);
                    outputStream << Connection->GetMyConnectionString();
                    outputStream.close();
                }

                if (m_ConnectedPeers.size() > 0)
                {
                    GenesisLiveRelayPacketConnectionStringUpdate packetConnectionStringUpdate = GenesisLiveRelayPacketConnectionStringUpdate();
                    packetConnectionStringUpdate.SetConnectionString(Connection->GetMyConnectionString());

                    this->BroadcastPacketsToPeers(&packetConnectionStringUpdate);
                }
            });

        m_AlwaysOpenConnection->SetConnectedToRemoteCallback(
            [this](GenesisLiveRelayConnection* Connection) -> void
            {
                m_Logger.Log("Debug", "Incoming connection: {}", Connection->GetConnection()->remoteAddress()->data());

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

    ash::AshResult GenesisLiveRelay::RemovePeer(GenesisPeerId PeerId, std::string Reason)
    {
        if (m_ConnectedPeers.contains(PeerId) == false)
        {
            return ash::AshResult(false, "Unknown peer");
        }

        GenesisLiveRelayPeerConnection* peer = m_ConnectedPeers.at(PeerId);
        m_ConnectedPeers.erase(PeerId);

        delete peer;

        // Add announcing to everybody here

        GenesisLiveRelayPacketConnectionClientLeft clientLeft = GenesisLiveRelayPacketConnectionClientLeft();

        clientLeft.SetPeerId(PeerId);
        clientLeft.SetReason(Reason);

        BroadcastPacketsToPeers(&clientLeft);

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLiveRelay::HandleMessage(GenesisLiveRelayPeerConnection* Connection, GenesisLiveRelayPacketBase* Packet)
    {
        switch (Packet->GetType())
        {
        case GenesisLiveRelayPacketType::CLIENT_CONNECT_REQUEST:
        {
            if (Connection->GetState() == GenesisLiveRelayPeerConnection::StateType::CONNECTING)
            {
                m_HasFirstPeerBeenConnected = true;

                GenesisLiveRelayPacketClientConnectRequest* connectRequest = reinterpret_cast<GenesisLiveRelayPacketClientConnectRequest*>(Packet);
                GenesisLiveRelayPacketClientConnectResponse connectResponse = GenesisLiveRelayPacketClientConnectResponse();

                Connection->SetName(fmt::format("{} #{}", connectRequest->GetClientName(), Connection->GetPeerId()));
                m_Logger.Log("Info", "Received connect request with username: {}", Connection->GetName());

                connectResponse.SetReason(GenesisLiveRelayPacketClientConnectResponse::ReasonType::FORCE_ASSIGN_TO_YOU);
                connectResponse.SetAssignedPeerId(Connection->GetPeerId());
                connectResponse.SetAssignedClientName(Connection->GetName());
                connectResponse.SetIsFirstToConnect(m_ConnectedPeers.size() <= 1);

                Connection->SetState(GenesisLiveRelayPeerConnection::StateType::AUTHED);

                Connection->SendPacket(&connectResponse);

                // Notify him of other peers

                ForeachPeer(
                    [this, Connection](GenesisPeerId PeerId, GenesisLiveRelayPeerConnection* Peer) -> void
                    {
                        GenesisLiveRelayPacketClientConnectResponse dummyConnectResponse = GenesisLiveRelayPacketClientConnectResponse();

                        dummyConnectResponse.SetAssignedPeerId(Peer->GetPeerId());
                        dummyConnectResponse.SetAssignedClientName(Peer->GetName());
                        dummyConnectResponse.SetReason(GenesisLiveRelayPacketClientConnectResponse::ReasonType::CLIENT_JOINED);

                        Connection->SendPacket(&dummyConnectResponse);
                    },
                    {Connection->GetPeerId()});

                // Broadcast to others

                connectResponse.SetReason(GenesisLiveRelayPacketClientConnectResponse::ReasonType::CLIENT_JOINED);

                BroadcastPacketsToPeers(&connectResponse, {Connection->GetPeerId()});

                // Inform client connection connect is done

                GenesisLiveRelayPacketConnectDone packetConnectDone = GenesisLiveRelayPacketConnectDone();
                Connection->SendPacket(&packetConnectDone);
            }

            break;
        }
        case GenesisLiveRelayPacketType::PING:
        {
            Connection->TouchPing();
            // Resend Ping Packet
            Connection->SendPacket(Packet);

            break;
        }
        case GenesisLiveRelayPacketType::CONNECTION_STRING_HINT_CONNECT:
        {
            GenesisLiveRelayPacketConnectionStringHintConnect* hintConnect = reinterpret_cast<GenesisLiveRelayPacketConnectionStringHintConnect*>(Packet);
            m_AlwaysOpenConnection->SetRemoteConnectionString(hintConnect->GetConnectionString());

            m_Logger.Log("Debug", "Received connection hint: {}", hintConnect->GetConnectionString());

            break;
        }
        case GenesisLiveRelayPacketType::BROADCAST:
        {
            GenesisLiveRelayPacketBroadcast* broadcast = reinterpret_cast<GenesisLiveRelayPacketBroadcast*>(Packet);

            if (broadcast->GetBuffer().IsAllocated() && broadcast->GetBuffer().GetSize() > 0)
            {
                // Avoid spoofing
                broadcast->SetSender(Connection->GetPeerId());

                BroadcastPacketsToPeers(broadcast, {Connection->GetPeerId()});
            }

            break;
        }
        default:
            m_Logger.Log("DebugError", "Received unknown packet type from client {}.", Connection->GetPeerId());
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

        // First Connection Initialization

        std::string connectionString = "";

        m_Logger.Log("Info", "Please paste in your invite code: ");
        std::cin >> connectionString;

        m_AlwaysOpenConnection->SetRemoteConnectionString(connectionString);

        // Main Loop

        while (m_ConnectedPeers.size() > 0 || (m_HasFirstPeerBeenConnected == false))
        {
            std::stack<std::pair<GenesisPeerId, std::string>> forceRemovalOfPeers = {};

            for (auto currentIterator : m_ConnectedPeers)
            {
                if (currentIterator.second->HasTimeouted(m_TimeoutDuration))
                {
                    forceRemovalOfPeers.push(std::make_pair(currentIterator.first, "Client has timeouted."));
                    m_Logger.Log("Error", "Removing peer {} because of timeout.", currentIterator.first);
                }
            }

            while (forceRemovalOfPeers.empty() == false)
            {
                GenesisPeerId currentRemovalPeerId = forceRemovalOfPeers.top().first;
                std::string currentRemovalReason = forceRemovalOfPeers.top().second;

                forceRemovalOfPeers.pop();

                this->RemovePeer(currentRemovalPeerId, currentRemovalReason);
            }

            std::this_thread::sleep_for(m_TimeoutDuration * 2.5);
            std::this_thread::yield();
        }

        m_Logger.Log("Exit", "Shutting down because no peers are connected.");
    }

} // namespace genesis::live