#include "AshObjects/AshString.h"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"

namespace genesis::live
{

    GenesisLiveRelayPacketClientConnectRequest::GenesisLiveRelayPacketClientConnectRequest() : GenesisLiveRelayPacketBase(), m_ClientName(), m_ClientKey()
    {
    }

    bool GenesisLiveRelayPacketClientConnectRequest::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString clientNameObject = ash::objects::AshAsciiString(), clientKeyObject = ash::objects::AshAsciiString();

        bool res = clientNameObject.Import(Stream);
        res &= clientKeyObject.Import(Stream);

        m_ClientName = clientNameObject.GetText();
        m_ClientKey = clientKeyObject.GetText();

        return Stream->IsOkay() && res;
    }

    bool GenesisLiveRelayPacketClientConnectRequest::Export(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString(m_ClientName).Export(Stream);
        ash::objects::AshAsciiString(m_ClientKey).Export(Stream);

        return Stream->IsOkay();
    }

} // namespace genesis::live