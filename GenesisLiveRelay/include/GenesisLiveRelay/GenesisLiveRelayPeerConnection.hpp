#ifndef _GENESISLIVERELAYPEERCONNECTION_HPP
#define _GENESISLIVERELAYPEERCONNECTION_HPP

#include "Ash/AshStepTimer.h"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"
#include <bits/chrono.h>
#include <chrono>

namespace genesis::live
{

    class GenesisLiveRelayPeerConnection : public GenesisLiveRelayConnection
    {
    public:
        enum StateType
        {
            INVALID = 0,
            CONNECTING = 1,
            AUTHED = 2
        };

        GenesisLiveRelayPeerConnection();

        inline GenesisPeerId GetPeerId()
        {
            return m_PeerId;
        }

        inline void SetPeerId(GenesisPeerId PeerId)
        {
            m_PeerId = PeerId;
        }

        inline std::chrono::milliseconds TouchPing()
        {
            m_LastPing = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            return m_LastPing;
        }

        inline void ForceTimeout()
        {
            m_LastPing = std::chrono::milliseconds(0);
        }

        inline std::chrono::milliseconds GetLastPing()
        {
            return m_LastPing;
        }

        inline bool HasTimeouted(std::chrono::milliseconds Duration)
        {
            return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - m_LastPing) > Duration;
        }

        inline StateType GetState()
        {
            return m_State;
        }

        inline void SetState(StateType State)
        {
            m_State = State;
        }

        inline std::string GetName()
        {
            return m_Name;
        }

        inline void SetName(std::string Name)
        {
            m_Name = Name;
        }

    protected:
        GenesisPeerId m_PeerId;
        std::chrono::milliseconds m_LastPing;
        StateType m_State;
        std::string m_Name;
    };

} // namespace genesis::live

#endif // !_GENESISLIVERELAYPEERCONNECTION_HPP