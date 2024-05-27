#include "Ash/AshBuffer.h"
#include "AshObjects/AshString.h"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"

namespace genesis::live
{

    static constexpr ash::AshSize smConnectionStringObjectTextLimitLength = 8192;

    GenesisLiveRelayPacketConnectionStringUpdate::GenesisLiveRelayPacketConnectionStringUpdate() : m_ConnectionString()
    {
    }

    bool GenesisLiveRelayPacketConnectionStringUpdate::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString connectionStringObject = ash::objects::AshAsciiString();
        connectionStringObject.SetTextLimitLength(smConnectionStringObjectTextLimitLength);

        if(connectionStringObject.Import(Stream) == false)
        {
            return false;
        }

        m_ConnectionString = connectionStringObject.GetText();

        return Stream->IsOkay();
    }

    bool GenesisLiveRelayPacketConnectionStringUpdate::Export(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString(m_ConnectionString).Export(Stream);

        return Stream->IsOkay();
    }

} // namespace genesis::live