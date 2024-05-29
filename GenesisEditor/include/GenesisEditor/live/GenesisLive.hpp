#ifndef _GENESISLIVE_HPP
#define _GENESISLIVE_HPP

#include "Ash/AshResult.h"
#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/live/GenesisLiveConnection.hpp"
#include "GenesisEditor/live/GenesisLivePackets.hpp"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include <chrono>
#include <functional>
#include <map>
#include <thread>

namespace genesis::live
{

    class GenesisLive
    {
    public:
        using sdProvideMainBundleCallback = std::function<GenesisBundle*()>;
        using sdBundleActionCallback = std::function<void(GenesisLiveConnectionPacketBundleAction* Action)>;
        using sdFlowActionCallback = std::function<void(GenesisLiveConnectionPacketFlowAction* Action)>;

        GenesisLive(utils::GenesisLogBox* LogBox, std::string Username);
        ~GenesisLive();

        ash::AshResult InitializeConnection(std::string RemoteInviteCode);
        ash::AshResult Reset();
        ash::AshResult HandleRelayMessage(GenesisLiveConnection* Connection, GenesisLiveRelayPacketBase* Packet);
        ash::AshResult HandleBroadcastMessage(GenesisPeerId Sender, GenesisLiveConnectionPacketBase* Packet);

        // Basically hints the relay server to launch a connection to the given remote connection (provided by the RemoteConnectionString)
        ash::AshResult InviteHintConnection(std::string RemoteConnectionString);
        ash::AshResult BroadcastPacketToPeers(GenesisLiveConnectionPacketBase* Packet);

        std::chrono::milliseconds TouchPing();
        GenesisPeerId GetHostPeerId();
        bool IsMyselfHost();

        inline bool HasBeenInitialized()
        {
            return m_Connection != nullptr;
        }

        inline GenesisLiveConnection* GetRelayConnection()
        {
            return m_Connection;
        }

        inline std::string GetLocalConnectionInviteCode()
        {
            return m_LocalConnectionInviteCode;
        }

        inline std::string GetRelayConnectionInviteCode()
        {
            return m_RelayConnectionInviteCode;
        }

        inline GenesisPeerId GetAssignedPeerId()
        {
            return m_AssignedPeerId;
        }

        inline std::string GetAssignedUsername()
        {
            return m_AssignedUsername;
        }

        inline const std::map<GenesisPeerId, std::string>& GetConnectedPeers()
        {
            return m_ConnectedPeers;
        }

        // Callbacks

        inline void SetProvideMainBundleCallback(sdProvideMainBundleCallback ProvideMainBundleCallback)
        {
            m_ProvideMainBundleCallback = std::move(ProvideMainBundleCallback);
        }

        inline void SetBundleActionCallback(sdBundleActionCallback BundleActionCallback)
        {
            m_BundleActionCallback = std::move(BundleActionCallback);
        }

        inline void SetFlowActionCallback(sdFlowActionCallback FlowActionCallback)
        {
            m_FlowActionCallback = std::move(FlowActionCallback);
        }

    private:
        void sRunnerThreadFunction();

    protected:
        ash::AshLogger m_Logger;
        std::jthread* m_RunnerThread;

        GenesisLiveConnection* m_Connection;
        std::string m_LocalConnectionInviteCode;
        std::string m_Username;

        GenesisPeerId m_AssignedPeerId;
        std::string m_AssignedUsername;

        std::chrono::milliseconds m_LastPing;

        bool m_RelayIsFirstToConnect;
        std::string m_RelayConnectionInviteCode;

        std::map<GenesisPeerId, std::string> m_ConnectedPeers;

        // Callbacks

        sdProvideMainBundleCallback m_ProvideMainBundleCallback;
        sdBundleActionCallback m_BundleActionCallback;
        sdFlowActionCallback m_FlowActionCallback;
    };

} // namespace genesis::live

#endif // !_GENESISLIVE_HPP