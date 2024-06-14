#include "GenesisEditor/live/GenesisLivePackets.hpp"

namespace genesis::live
{

    GenesisLiveConnectionPacketOperationUpdate::GenesisLiveConnectionPacketOperationUpdate():
        m_FlowName(),
        m_OperationId(-1),
        m_Buffer()
    {
    }

    GenesisLiveConnectionPacketOperationUpdate::GenesisLiveConnectionPacketOperationUpdate(std::string FlowName, operations::GenesisOperationId OperationId, ash::AshBuffer Buffer):
        m_FlowName(FlowName),
        m_OperationId(OperationId),
        m_Buffer(Buffer)
    {}

    bool GenesisLiveConnectionPacketOperationUpdate::Import(ash::AshStream* Stream)
    {
        return false;
    }
    
    bool GenesisLiveConnectionPacketOperationUpdate::Export(ash::AshStream* Stream)
    {
        return false;
    }
    

}