#include "GenesisLiveRelay/GenesisLiveRelayPeerConnection.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"

namespace genesis::live
{

    GenesisLiveRelayPeerConnection::GenesisLiveRelayPeerConnection():
        m_PeerId(),
        m_LastPing(),
        m_State(StateType::CONNECTING),
        // Relay is always the offerer
        GenesisLiveRelayConnection(InitializationConnectionWayType::OFFERER)
    {}

    

}