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

        inline bool HasBeenInitialized()
        {
            return m_Connection != nullptr;
        }

        inline GenesisLiveConnection* GetRelayConnection()
        {
            return m_Connection;
        }

        inline std::string GetConnectionInviteCode()
        {
            return m_ConnectionInviteCode;
        }
    private:
        void sRunnerThreadFunction();
    protected:
        ash::AshLogger m_Logger;
        std::jthread* m_RunnerThread;

        GenesisLiveConnection* m_Connection;
        std::string m_ConnectionInviteCode;
        std::string m_Username;
        
        GenesisPeerId m_AssignedPeerId;
        std::string m_AssignedUsername;
        
        std::chrono::milliseconds m_LastPing;
    };

} // namespace genesis::live

#endif // !_GENESISLIVE_HPP