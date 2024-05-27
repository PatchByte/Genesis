#include "GenesisEditor/live/GenesisLive.hpp"
#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/live/GenesisLiveConnection.hpp"
#include "GenesisEditor/live/GenesisLivePackets.hpp"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConfig.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <thread>

namespace genesis::live
{

    GenesisLive::GenesisLive(utils::GenesisLogBox* LogBox, std::string Username)
        : m_Logger("GenesisLive", {}), m_Connection(nullptr), m_LocalConnectionInviteCode(), m_Username(Username), m_AssignedPeerId(), m_AssignedUsername(), m_RunnerThread(), m_LastPing()
    {
        m_Logger.AddLoggerPassage(LogBox->CreatePassage());

        std::cout << Username << std::endl;
    }

    GenesisLive::~GenesisLive()
    {
        this->Reset();
    }

    ash::AshResult GenesisLive::InitializeConnection(std::string RemoteInviteCode)
    {
        this->Reset();

        m_Connection = new GenesisLiveConnection();
        m_Connection->SetGatheredServerReflexiveCandidateCallback([this](GenesisLiveRelayConnection* Connection) -> void { m_LocalConnectionInviteCode = Connection->GetMyConnectionString(); });

        m_Connection->SetConnectedToRemoteCallback([this](GenesisLiveRelayConnection* Connection) -> void { m_Logger.Log("Info", "Connected to relay"); });

        m_Connection->SetDisconnectedFromRemoteCallback([this](GenesisLiveRelayConnection* Connection) -> void { m_Logger.Log("Info", "Disconnected from relay"); });

        m_Connection->SetErrorCallback([this](GenesisLiveRelayConnection* Connection, std::string ErrorMessage) -> void { m_Logger.Log("Received an error from the relay. {}", ErrorMessage); });

        m_Connection->SetReadyToSendMessageCallback(
            [this](GenesisLiveRelayConnection* Connection) -> void
            {
                m_Logger.Log("Info", "Channel is open, negotiating.");

                this->TouchPing();

                GenesisLiveRelayPacketClientConnectRequest connectRequest = GenesisLiveRelayPacketClientConnectRequest();
                connectRequest.SetClientName(m_Username);

                Connection->SendPacket(&connectRequest);
            });

        m_Connection->SetMessageCallback(
            [this](GenesisLiveRelayConnection* ConnectionUnCast, ash::AshBuffer* Buffer) -> void
            {
                GenesisLiveRelayPacketBase* packetDeserialized = nullptr;

                if (auto res = GenesisLiveRelayPacketUtils::sfDeserializePacket("GenesisRelayPacket", Buffer); res.HasError())
                {
                    m_Logger.Log("Info", "Failed to deserialize packet.", res.GetMessage());

                    return;
                }
                else
                {
                    packetDeserialized = res.GetResult();
                }

                this->HandleRelayMessage(reinterpret_cast<GenesisLiveConnection*>(ConnectionUnCast), packetDeserialized);

                delete packetDeserialized;
            });

        m_RunnerThread = new std::jthread(
            [this, RemoteInviteCode]()
            {
                if (auto res = m_Connection->InitializeConnection(); res.HasError())
                {
                    m_Logger.Log("Error", "Failed to initialize connection.");
                    return;
                }

                m_Connection->SetRemoteConnectionString(RemoteInviteCode);

                this->sRunnerThreadFunction();
            });

        m_Logger.Log("Info", "Connecting to relay");

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLive::Reset()
    {
        if (m_RunnerThread)
        {
            m_RunnerThread->request_stop();

            delete m_RunnerThread;
            m_RunnerThread = nullptr;
        }

        if (m_Connection)
        {
            delete m_Connection;
            m_Connection = nullptr;
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLive::HandleRelayMessage(GenesisLiveConnection* Connection, GenesisLiveRelayPacketBase* Packet)
    {
        switch (Packet->GetType())
        {
        case GenesisLiveRelayPacketType::CLIENT_CONNECT_RESPONSE:
        {
            GenesisLiveRelayPacketClientConnectResponse* response = reinterpret_cast<GenesisLiveRelayPacketClientConnectResponse*>(Packet);

            if (response->GetReason() == GenesisLiveRelayPacketClientConnectResponse::ReasonType::FORCE_ASSIGN_TO_YOU)
            {
                m_AssignedPeerId = response->GetAssignedPeerId();
                m_AssignedUsername = response->GetAssignedClientName();

                m_Logger.Log("Info", "Negotiation successful. My assigned Peer-Id: {}. My assigned Name: {}", m_AssignedPeerId, m_AssignedUsername);

                GenesisLiveRelayPacketPing packet = GenesisLiveRelayPacketPing();
                packet.SetChallenge(rand());

                // Send first ping packet.
                m_Connection->SendPacket(&packet);

                m_Connection->SetAuthed(true);

                if (response->IsFirstToConnect() == false)
                {
                    // Ask other peers to provide flow.

                    GenesisLiveConnectionPacketRequestSerializedFile requestSerializedFilePacket = GenesisLiveConnectionPacketRequestSerializedFile();
                    BroadcastPacketToPeers(&requestSerializedFilePacket);
                }
            }
            else if (response->GetReason() == GenesisLiveRelayPacketClientConnectResponse::ReasonType::CLIENT_JOINED)
            {
                if (m_ConnectedPeers.contains(response->GetAssignedPeerId()) == false)
                {
                    m_ConnectedPeers.emplace(response->GetAssignedPeerId(), response->GetAssignedClientName());

                    m_Logger.Log("Info", "Peer {} joined.", response->GetAssignedClientName());
                }
                else
                {
                    m_Logger.Log("Error", "Received GenesisLiveRelayPacketClientConnectResponse from an already existing peer.");
                }
            }

            break;
        }
        case GenesisLiveRelayPacketType::PING:
        {
            break;
        }
        case GenesisLiveRelayPacketType::CONNECTION_STRING_UPDATE:
        {
            // This receives the connection string (aka Invite Code) of the relay server.

            GenesisLiveRelayPacketConnectionStringUpdate* connectionStringUpdate = reinterpret_cast<GenesisLiveRelayPacketConnectionStringUpdate*>(Packet);
            m_RelayConnectionInviteCode = connectionStringUpdate->GetConnectionString();

            break;
        }
        case GenesisLiveRelayPacketType::CLIENT_LEFT:
        {
            GenesisLiveRelayPacketConnectionClientLeft* clientLeft = reinterpret_cast<GenesisLiveRelayPacketConnectionClientLeft*>(Packet);

            if (m_ConnectedPeers.contains(clientLeft->GetPeerId()))
            {
                m_Logger.Log("Info", "Peer {} disconnected. {}", m_ConnectedPeers.at(clientLeft->GetPeerId()), clientLeft->GetReason());

                m_ConnectedPeers.erase(clientLeft->GetPeerId());
            }
            else
            {
                m_Logger.Log("Error", "Server sent client left packet with a non existing peer id.");
            }

            break;
        }
        case GenesisLiveRelayPacketType::BROADCAST:
        {
            GenesisLiveRelayPacketBroadcast* broadcast = reinterpret_cast<GenesisLiveRelayPacketBroadcast*>(Packet);

            if (auto res = GenesisLiveConnectionPacketUtils::sfDeserializePacket("GenesisLiveConnectionPacket", &broadcast->GetBuffer()); res.HasError())
            {
                m_Logger.Log("Error", "Failed to deserialize packet from peer {}. {}", broadcast->GetSender(), res.GetMessage());
            }
            else
            {
                GenesisLiveConnectionPacketBase* packet = res.GetResult();

                HandleBroadcastMessage(broadcast->GetSender(), packet);

                delete packet;
            }

            break;
        }
        default:
            break;
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLive::HandleBroadcastMessage(GenesisPeerId Sender, GenesisLiveConnectionPacketBase* Packet)
    {
        switch (Packet->GetType())
        {
        case GenesisLiveConnectionPacketType::REQUEST_GENESIS_SERIALIZED_FILE:
        {
            if (this->IsMyselfHost())
            {
                GenesisBundle* mainBundle = m_ProvideMainBundleCallback();
                GenesisLiveConnectionPacketResponseSerializedFile responsePacket = GenesisLiveConnectionPacketResponseSerializedFile();
                ash::AshStreamExpandableExportBuffer serializedFileResponseStream = ash::AshStreamExpandableExportBuffer();
                
                if(mainBundle->Export(&serializedFileResponseStream) == false)
                {
                    m_Logger.Log("Error", "Failed to serialize bundle.");
                }
                else
                {
                    if(auto buffer = serializedFileResponseStream.MakeCopyOfBuffer())
                    {
                        responsePacket.SetSerializedFile(*buffer);
                        BroadcastPacketToPeers(&responsePacket),

                        delete buffer;
                    }
                }
            }

            break;
        }
        case GenesisLiveConnectionPacketType::RESPONSE_GENESIS_SERIALIZED_FILE:
        {
            if (this->GetHostPeerId() == Sender)
            {
                GenesisLiveConnectionPacketResponseSerializedFile* serializedFileResponse = reinterpret_cast<GenesisLiveConnectionPacketResponseSerializedFile*>(Packet);
                GenesisBundle* mainBundle = m_ProvideMainBundleCallback();

                ash::AshStreamStaticBuffer serializedFileResponseStream = ash::AshStreamStaticBuffer(&serializedFileResponse->GetSerializedFile(), ash::AshStreamMode::READ);

                if (mainBundle->Import(&serializedFileResponseStream) == false)
                {
                    m_Logger.Log("Error", "Failed to deserialize received bundle.");
                }
                else
                {
                    m_Logger.Log("Info", "Applied received bundle.");
                }
            }
            break;
        }
        default:
            m_Logger.Log("Error", "Received unknown live connection packet by Peer {}.", Sender);
            break;
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLive::InviteHintConnection(std::string RemoteConnectionString)
    {
        GenesisLiveRelayPacketConnectionStringHintConnect packetHintConnect = GenesisLiveRelayPacketConnectionStringHintConnect();
        packetHintConnect.SetConnectionString(RemoteConnectionString);

        return m_Connection->SendPacket(&packetHintConnect);
    }

    ash::AshResult GenesisLive::BroadcastPacketToPeers(GenesisLiveConnectionPacketBase* Packet)
    {
        if (auto res = GenesisLiveConnectionPacketUtils::sfSerializePacket("GenesisLiveConnectionPacket", Packet); res.HasError())
        {
            return ash::AshResult(false, res.GetMessage());
        }
        else
        {
            GenesisLiveRelayPacketBroadcast packetBroadcast = GenesisLiveRelayPacketBroadcast();

            packetBroadcast.SetSender(m_AssignedPeerId);
            packetBroadcast.SetBuffer(*res.GetResult());

            return m_Connection->SendPacket(&packetBroadcast);
        }

        return ash::AshResult(false);
    }

    std::chrono::milliseconds GenesisLive::TouchPing()
    {
        m_LastPing = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        return m_LastPing;
    }

    GenesisPeerId GenesisLive::GetHostPeerId()
    {
        GenesisPeerId hostPeerId = m_AssignedPeerId;

        for (auto currentIterator : m_ConnectedPeers)
        {
            hostPeerId = std::min(hostPeerId, currentIterator.first);
        }

        return hostPeerId;
    }

    bool GenesisLive::IsMyselfHost()
    {
        return GetHostPeerId() == m_AssignedPeerId;
    }

    void GenesisLive::sRunnerThreadFunction()
    {
        TouchPing();

        while (m_RunnerThread->get_stop_token().stop_requested() == false)
        {
            auto lastPingDurations = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - m_LastPing);

            if (lastPingDurations > GenesisLiveRelayConfig::smTimeoutDuration && m_Connection->IsAuthed())
            {
                TouchPing();

                GenesisLiveRelayPacketPing packet = GenesisLiveRelayPacketPing();
                packet.SetChallenge(rand());

                m_Connection->SendPacket(&packet);
            }

            std::this_thread::yield();
        }
    }

} // namespace genesis::live