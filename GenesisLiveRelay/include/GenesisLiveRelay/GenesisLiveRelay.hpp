#ifndef _GENESISLIVERELAY_HPP
#define _GENESISLIVERELAY_HPP

#include "Ash/AshResult.h"
#include "AshLogger/AshLogger.h"
#include "GenesisLiveRelay/GenesisLiveRelayPeerConnection.hpp"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include <chrono>
#include <functional>
#include <map>
#include <vector>

namespace genesis::live
{

    class GenesisLiveRelay
    {
    public:
        GenesisLiveRelay();

        void Run();

    public:
        using sdForeachPeerDelegate = std::function<void(GenesisPeerId,GenesisLiveRelayPeerConnection*)>;

        ash::AshResult AllocateNewAlwaysOpenConnection();
        ash::AshResult MoveAlwaysOpenConnectionToNextStage();

        ash::AshResult RemovePeer(GenesisPeerId PeerId, std::string Reason = "");
        ash::AshResult HandleMessage(GenesisLiveRelayPeerConnection* Connection, GenesisLiveRelayPacketBase* Packet);
        ash::AshResult ForeachPeer(sdForeachPeerDelegate Function, std::vector<GenesisPeerId> AvoidPeers = {});
        ash::AshResult BroadcastPacketsToPeers(GenesisLiveRelayPacketBase* Packet, std::vector<GenesisPeerId> AvoidPeers = {});
    protected:
        ash::AshLogger m_Logger;

        GenesisPeerId m_PeerCounter;
        GenesisLiveRelayPeerConnection* m_AlwaysOpenConnection;

        std::map<GenesisPeerId, GenesisLiveRelayPeerConnection*> m_ConnectedPeers;

        // Config

        std::chrono::milliseconds m_TimeoutDuration;
    };

} // namespace genesis::live

#endif // !_GENESISLIVERELAY_HPPf