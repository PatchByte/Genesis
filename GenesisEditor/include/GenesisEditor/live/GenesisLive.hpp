#ifndef _GENESISLIVE_HPP
#define _GENESISLIVE_HPP

#include "Ash/AshResult.h"
#include "Ash/AshStepTimer.h"
#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/live/GenesisLiveConnection.hpp"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"
#include <chrono>
#include <map>
#include <thread>

namespace genesis::live
{

    class GenesisLive
    {
    public:
        GenesisLive(utils::GenesisLogBox* LogBox, std::string Username);
        ~GenesisLive();

        ash::AshResult InitializeConnection(std::string RemoteInviteCode);
        ash::AshResult Reset();
        ash::AshResult HandleMessage(GenesisLiveConnection* Connection, GenesisLiveRelayPacketBase* Packet);

        std::chrono::milliseconds TouchPing();

        // Basically hints the relay server to launch a connection to the given remote connection (provided by the RemoteConnectionString)
        ash::AshResult InviteHintConnection(std::string RemoteConnectionString);

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

        std::string m_RelayConnectionInviteCode;

        std::map<GenesisPeerId, std::string> m_ConnectedPeers;
    };

} // namespace genesis::live

#endif // !_GENESISLIVE_HPP