#include "GenesisEditor/live/GenesisLiveConnection.hpp"
#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"

namespace genesis::live
{

    GenesisLiveConnection::GenesisLiveConnection():
        m_Authed(false),
        GenesisLiveRelayConnection(InitializationConnectionWayType::ANSWERER)
    {
    }

}