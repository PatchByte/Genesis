#include "AshObjects/AshString.h"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"

namespace genesis::live
{

    GenesisLiveRelayPacketClientConnectResponse::GenesisLiveRelayPacketClientConnectResponse() : m_AssignedClientName(), m_AssignedPeerId(0), m_Reason(ReasonType::INVALID)
    {
    }

    bool GenesisLiveRelayPacketClientConnectResponse::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString assignedClientNameObject = ash::objects::AshAsciiString();

        if (assignedClientNameObject.Import(Stream) == false)
        {
            return false;
        }

        m_AssignedClientName = assignedClientNameObject.GetText();
        
        m_AssignedPeerId = Stream->Read<GenesisPeerId>();
        m_Reason = static_cast<ReasonType>(Stream->Read<int>());
        m_IsFirstToConnect = Stream->Read<bool>();

        return Stream->IsOkay();
    }

    bool GenesisLiveRelayPacketClientConnectResponse::Export(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString(m_AssignedClientName).Export(Stream);

        Stream->Write<GenesisPeerId>(m_AssignedPeerId);
        Stream->Write(static_cast<int>(m_Reason));
        Stream->Write<bool>(m_IsFirstToConnect);

        return Stream->IsOkay();
    }

} // namespace genesis::live