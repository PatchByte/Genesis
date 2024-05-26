#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"
#include "Ash/AshBase64.h"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "Ash/AshSerializer.h"
#include "Ash/AshStream.h"
#include "AshObjects/AshString.h"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include "nlohmann/json_fwd.hpp"
#include "rtc/candidate.hpp"
#include "rtc/configuration.hpp"
#include "rtc/description.hpp"
#include "rtc/peerconnection.hpp"
#include <cstddef>
#include <future>
#include <vector>

#include <nlohmann/json.hpp>

namespace genesis::live
{

    static constexpr std::string_view smGenesisDataChannelName = "GenesisDataChannel";

    GenesisLiveRelayConnection::GenesisLiveRelayConnection(InitializationConnectionWayType InitializationConnectionWay)
        : m_InitializationConnectionWay(InitializationConnectionWay), m_Configuration(), m_Connection(nullptr), m_IsConnected(false), m_IsWaiting(false), m_LocalDescription(), m_LocalCandidates(),
          m_GatheredServerReflexiveCandidateCallback([](...) {}), m_ConnectedToRemoteCallback([](...) {}), m_DisconnectedFromRemoteCallback([](...) {}), m_ErrorCallback([](...) {}),
          m_MessageCallback([](...) {}), m_ReadyToSendMessageCallback([](...) {})
    {
        this->Reset();
    }

    GenesisLiveRelayConnection::~GenesisLiveRelayConnection()
    {
        this->Reset();
    }

    ash::AshResult GenesisLiveRelayConnection::InitializeConnection()
    {
        Reset();

        m_Connection = new rtc::PeerConnection(m_Configuration);

        m_Connection->onLocalDescription([this](rtc::Description Description) -> void { m_LocalDescription = Description; });
        m_Connection->onLocalCandidate(
            [this](rtc::Candidate Candidate) -> void
            {
                m_LocalCandidates.push_back(std::string(Candidate));

                if (Candidate.type() == rtc::Candidate::Type::ServerReflexive)
                {
                    auto routine = std::async(std::launch::async, m_GatheredServerReflexiveCandidateCallback, this);
                }
            });

        m_IsWaiting = false;
        m_IsConnected = false;

        m_Connection->onStateChange(
            [this](rtc::PeerConnection::State State) -> void
            {
                switch (State)
                {
                case rtc::PeerConnection::State::New:
                    break;
                case rtc::PeerConnection::State::Connecting:
                    break;
                case rtc::PeerConnection::State::Connected:
                {
                    m_IsWaiting = false;
                    m_IsConnected = true;
                    auto routine = std::async(std::launch::async, m_ConnectedToRemoteCallback, this);
                    break;
                }
                case rtc::PeerConnection::State::Disconnected:
                {
                    m_IsWaiting = false;
                    m_IsConnected = false;
                    auto routine = std::async(std::launch::async, m_DisconnectedFromRemoteCallback, this);
                    break;
                }
                case rtc::PeerConnection::State::Failed:
                {
                    auto routine = std::async(std::launch::async, m_ErrorCallback, this, "Unknown");
                    break;
                }
                case rtc::PeerConnection::State::Closed:
                    break;
                }
            });

        m_Connection->onGatheringStateChange(
            [this](rtc::PeerConnection::GatheringState GatheringState) -> void
            {
                switch (GatheringState)
                {
                case rtc::PeerConnection::GatheringState::New:
                case rtc::PeerConnection::GatheringState::InProgress:
                case rtc::PeerConnection::GatheringState::Complete:
                    m_IsWaiting = true;
                    break;
                }
            });

        if (m_InitializationConnectionWay == InitializationConnectionWayType::OFFERER)
        {
            m_DataChannel = m_Connection->createDataChannel(smGenesisDataChannelName.data());

            m_DataChannel->onOpen([this]() { auto routine = std::async(std::launch::async, m_ReadyToSendMessageCallback, this); });

            m_DataChannel->onMessage(
                [this](std::vector<std::byte> Bytes) -> void
                {
                    ash::AshBuffer* buffer = new ash::AshBuffer(Bytes.data(), Bytes.size());
                    auto routine = std::async(std::launch::async, m_MessageCallback, this, buffer);
                },
                [this](std::string) -> void {});
        }
        else if (m_InitializationConnectionWay == InitializationConnectionWayType::ANSWERER)
        {
            m_Connection->onDataChannel(
                [this](std::shared_ptr<rtc::DataChannel> DataChannel) -> void
                {
                    if (DataChannel->label() == smGenesisDataChannelName)
                    {
                        m_DataChannel = DataChannel;

                        m_DataChannel->onOpen([this]() { auto routine = std::async(std::launch::async, m_ReadyToSendMessageCallback, this); });

                        m_DataChannel->onMessage(
                            [this](std::vector<std::byte> Bytes) -> void
                            {
                                ash::AshBuffer* buffer = new ash::AshBuffer(Bytes.data(), Bytes.size());
                                auto routine = std::async(std::launch::async, m_MessageCallback, this, buffer);
                            },
                            [this](std::string) -> void {});
                    }
                });
        }

        return ash::AshResult(true);
    }

    bool GenesisLiveRelayConnection::Reset()
    {
        m_IsConnected = false;
        m_IsWaiting = false;

        if (m_Connection)
        {
            delete m_Connection;
        }

        m_Configuration = rtc::Configuration();

        m_Configuration.iceServers = std::vector<rtc::IceServer>{
            rtc::IceServer("stun.l.google.com:19302"),  rtc::IceServer("stun1.l.google.com:19302"), rtc::IceServer("stun2.l.google.com:19302"),
            rtc::IceServer("stun3.l.google.com:19302"), rtc::IceServer("stun4.l.google.com:19302"),
        };

        m_LocalCandidates.clear();

        return true;
    }

    std::string GenesisLiveRelayConnection::GetMyConnectionString()
    {
        nlohmann::json data = nlohmann::json();

        data["description"] = m_LocalDescription;
        data["candidates"] = m_LocalCandidates;

        std::string dataString = data.dump(-1);
        auto dataStringBuffer = ash::AshBuffer(dataString.data(), dataString.length() + 1);

        return ash::AshBase64::Encode(&dataStringBuffer);
    }

    ash::AshResult GenesisLiveRelayConnection::SetRemoteConnectionString(std::string ConnectionStringEncoded)
    {
        ash::AshBuffer* dataStringDecodedBuffer = ash::AshBase64::Decode(ConnectionStringEncoded);
        std::string dataStringDecoded = dataStringDecodedBuffer->GetBuffer<char>();

        nlohmann::json data = nlohmann::json::parse(dataStringDecoded);

        m_Connection->setRemoteDescription(data["description"].get<std::string>());

        for (nlohmann::json currentCandidate : data["candidates"])
        {
            m_Connection->addRemoteCandidate(currentCandidate.get<std::string>());
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLiveRelayConnection::SendBytes(ash::AshBuffer* Buffer)
    {
        if (m_DataChannel.get() == nullptr)
        {
            return ash::AshResult(false, "Data channel has not been opened yet");
        }

        return ash::AshResult(m_DataChannel->send(reinterpret_cast<std::byte*>(Buffer->GetBytes()), Buffer->GetSize()));
    }

    ash::AshResult GenesisLiveRelayConnection::SendPacket(GenesisLiveRelayPacketBase* Packet)
    {
        ash::AshBuffer* packetSerializedBuffer = nullptr;

        if (auto res = GenesisLiveRelayPacketUtils::sfSerializePacket("GenesisRelayPacket", Packet); res.HasError())
        {
            return ash::AshResult(false, res.GetMessage());
        }
        else
        {
            packetSerializedBuffer = res.GetResult();
        }

        auto res = SendBytes(packetSerializedBuffer);

        delete packetSerializedBuffer;

        return res;
    }

} // namespace genesis::live