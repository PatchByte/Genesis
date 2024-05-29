#include "AshObjects/AshString.h"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"

namespace genesis::live
{

    GenesisLiveRelayPacketConnectionClientLeft::GenesisLiveRelayPacketConnectionClientLeft() : m_PeerId(-1), m_Reason()
    {
    }

    bool GenesisLiveRelayPacketConnectionClientLeft::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString reasonObject = ash::objects::AshAsciiString();

        m_PeerId = Stream->Read<GenesisPeerId>();

        if (reasonObject.Import(Stream) == false)
        {
            return false;
        }

        m_Reason = reasonObject.GetText();

        return Stream->IsOkay();
    }

    bool GenesisLiveRelayPacketConnectionClientLeft::Export(ash::AshStream* Stream)
    {
        Stream->Write<GenesisPeerId>(m_PeerId);
        ash::objects::AshAsciiString(m_Reason).Export(Stream);

        return Stream->IsOkay();
    }

} // namespace genesis::live