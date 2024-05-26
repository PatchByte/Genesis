#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "GenesisLiveShared/GenesisLiveHeader.hpp"

namespace genesis::live
{

    GenesisLiveRelayPacketBase* GenesisLiveRelayPacketUtils::sfCreateRelayPacketByType(GenesisLiveRelayPacketType Type)
    {
        switch (Type)
        {
        case GenesisLiveRelayPacketType::CLIENT_CONNECT_REQUEST:
            return new GenesisLiveRelayPacketClientConnectRequest();
        case GenesisLiveRelayPacketType::CLIENT_CONNECT_RESPONSE:
            return new GenesisLiveRelayPacketClientConnectResponse();
        case GenesisLiveRelayPacketType::PING:
            return new GenesisLiveRelayPacketPing();
        default:
            return nullptr;
        }
    }

    ash::AshCustomResult<ash::AshBuffer*> GenesisLiveRelayPacketUtils::sfSerializePacket(std::string Identification, GenesisLiveRelayPacketBase* Packet)
    {
        ash::AshStreamExpandableExportBuffer bufferStream = ash::AshStreamExpandableExportBuffer();
        GenesisLivePacketHeader packetHeader = GenesisLivePacketHeader(Identification, static_cast<unsigned int>(Packet->GetType()));

        if(packetHeader.Export(&bufferStream) == false)
        {
            return ash::AshCustomResult<ash::AshBuffer*>(false, "Failed to export packet header.");
        }

        if(Packet->Export(&bufferStream) == false)
        {
            return ash::AshCustomResult<ash::AshBuffer*>(false, "Failed to export packet");
        }

        return ash::AshCustomResult<ash::AshBuffer*>(true).AttachResult(bufferStream.MakeCopyOfBuffer());
    }

    ash::AshCustomResult<GenesisLiveRelayPacketBase*> GenesisLiveRelayPacketUtils::sfDeserializePacket(std::string Identification, ash::AshBuffer* Buffer)
    {
        ash::AshStreamStaticBuffer stream = ash::AshStreamStaticBuffer(Buffer, ash::AshStreamMode::READ);
        GenesisLivePacketHeader packetHeader = GenesisLivePacketHeader(Identification);

        if(packetHeader.Import(&stream) == false)
        {
            return ash::AshCustomResult<GenesisLiveRelayPacketBase*>(false, "Failed to import header.");
        }

        if(packetHeader.IsIdentification(Identification) == false)
        {
            return ash::AshCustomResult<GenesisLiveRelayPacketBase*>(false, "Packet header identification does not match.");
        }

        GenesisLiveRelayPacketBase* packet = sfCreateRelayPacketByType(packetHeader.GetTypeAs<GenesisLiveRelayPacketType>());

        if(packet == nullptr)
        {
            return ash::AshCustomResult<GenesisLiveRelayPacketBase*>(false, "Failed to create packet by type.");
        }

        if(packet->Import(&stream) == false)
        {
            return ash::AshCustomResult<GenesisLiveRelayPacketBase*>(false, "Failed to deserialize packet.");
        }

        return ash::AshCustomResult<GenesisLiveRelayPacketBase*>(true).AttachResult(packet);
    }

} // namespace genesis::live