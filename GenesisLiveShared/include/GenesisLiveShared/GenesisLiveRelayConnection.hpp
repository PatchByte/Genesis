#ifndef _GENESISLIVERELAYCONNECTION_HPP
#define _GENESISLIVERELAYCONNECTION_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include "rtc/configuration.hpp"
#include "rtc/peerconnection.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace genesis::live
{

    class GenesisLiveRelayConnection
    {
    public:
        enum class InitializationConnectionWayType
        {
            INVALID = 0,
            OFFERER = 1,
            ANSWERER = 2,
        };

        using sdSimpleDelegate = std::function<void(GenesisLiveRelayConnection*)>;
        using sdErrorDelegate = std::function<void(GenesisLiveRelayConnection*, std::string Error)>;
        using sdMessageDelegate = std::function<void(GenesisLiveRelayConnection*, ash::AshBuffer* Buffer)>;

        GenesisLiveRelayConnection(InitializationConnectionWayType InitializationConnectionWay);
        virtual ~GenesisLiveRelayConnection();

        ash::AshResult InitializeConnection();
        bool Reset();

        std::string GetMyConnectionString();
        ash::AshResult SetRemoteConnectionString(std::string ConnectionString);

        inline ash::AshResult SendBytes(ash::AshBuffer& Buffer)
        {
            return this->SendBytes(&Buffer);
        }

        ash::AshResult SendBytes(ash::AshBuffer* Buffer);
        ash::AshResult SendPacket(GenesisLiveRelayPacketBase* Packet);

        // Important Get / Setters

        inline void SetGatheredServerReflexiveCandidateCallback(sdSimpleDelegate GatheredServerReflexiveCandidateCallback)
        {
            m_GatheredServerReflexiveCandidateCallback = std::move(GatheredServerReflexiveCandidateCallback);
        }

        inline void SetConnectedToRemoteCallback(sdSimpleDelegate ConnectedToRemoteCallback)
        {
            m_ConnectedToRemoteCallback = std::move(ConnectedToRemoteCallback);
        }

        inline void SetDisconnectedFromRemoteCallback(sdSimpleDelegate DisconnectedFromRemoteCallback)
        {
            m_DisconnectedFromRemoteCallback = std::move(DisconnectedFromRemoteCallback);
        }

        inline void SetErrorCallback(sdErrorDelegate ErrorCallback)
        {
            m_ErrorCallback = std::move(ErrorCallback);
        }

        inline void SetMessageCallback(sdMessageDelegate MessageCallback)
        {
            m_MessageCallback = std::move(MessageCallback);
        }

        inline void SetReadyToSendMessageCallback(sdSimpleDelegate ReadyToSendMessageCallback)
        {
            m_ReadyToSendMessageCallback = std::move(ReadyToSendMessageCallback);
        }

        inline bool IsWaiting()
        {
            return m_IsWaiting;
        }

        inline bool IsConnected()
        {
            return m_IsConnected;
        }

        inline rtc::PeerConnection* GetConnection()
        {
            return m_Connection;
        }

    protected:
        InitializationConnectionWayType m_InitializationConnectionWay;

        rtc::Configuration m_Configuration;
        rtc::PeerConnection* m_Connection;

        bool m_IsWaiting : 1;
        bool m_IsConnected : 1;

        std::string m_LocalDescription;
        std::vector<std::string> m_LocalCandidates;

        std::shared_ptr<rtc::DataChannel> m_DataChannel;

        // Delegates and Callbacks

        sdSimpleDelegate m_GatheredServerReflexiveCandidateCallback;
        sdSimpleDelegate m_ConnectedToRemoteCallback;
        sdSimpleDelegate m_DisconnectedFromRemoteCallback;
        sdErrorDelegate m_ErrorCallback;
        sdMessageDelegate m_MessageCallback;
        sdSimpleDelegate m_ReadyToSendMessageCallback;
    };

} // namespace genesis::live

#endif // !_GENESISLIVERELAYCONNECTION_HPP