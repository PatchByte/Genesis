#include "GenesisEditor/live/GenesisLive.hpp"
#include "Ash/AshResult.h"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/live/GenesisLiveConnection.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConfig.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include <chrono>
#include <cstdlib>
#include <thread>

namespace genesis::live
{

    GenesisLive::GenesisLive(utils::GenesisLogBox* LogBox, std::string Username)
        : m_Logger("GenesisLive", {}), m_Connection(nullptr), m_ConnectionInviteCode(), m_Username(Username), m_AssignedPeerId(), m_AssignedUsername(), m_RunnerThread(), m_LastPing()
    {
        m_Logger.AddLoggerPassage(LogBox->CreatePassage());

        std::cout << Username << std::endl;
    }

    GenesisLive::~GenesisLive()
    {
    }

    ash::AshResult GenesisLive::InitializeConnection(std::string RemoteInviteCode)
    {
        this->Reset();

        m_Connection = new GenesisLiveConnection();
        m_Connection->SetGatheredServerReflexiveCandidateCallback([this](GenesisLiveRelayConnection* Connection) -> void { m_ConnectionInviteCode = Connection->GetMyConnectionString(); });

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

                this->HandleMessage(reinterpret_cast<GenesisLiveConnection*>(ConnectionUnCast), packetDeserialized);

                delete packetDeserialized;
            });

        if (auto res = m_Connection->InitializeConnection(); res.HasError())
        {
            return std::move(res);
        }

        m_RunnerThread = new std::jthread([this] () {
            this->sRunnerThreadFunction();
        });

        m_Logger.Log("Info", "Connecting to relay");

        return m_Connection->SetRemoteConnectionString(RemoteInviteCode);
    }

    ash::AshResult GenesisLive::Reset()
    {
        if (m_RunnerThread)
        {
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

    ash::AshResult GenesisLive::HandleMessage(GenesisLiveConnection* Connection, GenesisLiveRelayPacketBase* Packet)
    {
        switch (Packet->GetType())
        {
        case GenesisLiveRelayPacketType::CLIENT_CONNECT_RESPONSE:
        {
            GenesisLiveRelayPacketClientConnectResponse* response = reinterpret_cast<GenesisLiveRelayPacketClientConnectResponse*>(Packet);

            m_AssignedPeerId = response->GetAssignedPeerId();
            m_AssignedUsername = response->GetAssignedClientName();


            m_Logger.Log("Info", "Negotiation successful. My assigned Peer-Id: {}. My assigned Name: {}", m_AssignedPeerId, m_AssignedUsername);

            GenesisLiveRelayPacketPing packet = GenesisLiveRelayPacketPing();
            packet.SetChallenge(rand());

            m_Connection->SendPacket(&packet);

            m_Connection->SetAuthed(true);
            break;
        }
        case GenesisLiveRelayPacketType::PING:
        case GenesisLiveRelayPacketType::FORWARD_FROM:
        case GenesisLiveRelayPacketType::CLIENT_LEFT:
            break;
        default:
            break;
        }

        return ash::AshResult(true);
    }

    std::chrono::milliseconds GenesisLive::TouchPing()
    {
        m_LastPing = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        return m_LastPing;
    }

    void GenesisLive::sRunnerThreadFunction()
    {
        TouchPing();

        while (m_RunnerThread->get_stop_token().stop_requested() == false)
        {
            auto lastPingDurations = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - m_LastPing);

            if(lastPingDurations > GenesisLiveRelayConfig::smTimeoutDuration && m_Connection->IsAuthed())
            {
                TouchPing();

                GenesisLiveRelayPacketPing packet = GenesisLiveRelayPacketPing();
                packet.SetChallenge(rand());

                m_Connection->SendPacket(&packet);

                std::cout << "Sending" << std::endl;
            }

            std::this_thread::yield();
        }
    }

} // namespace genesis::live