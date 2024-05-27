#include "GenesisEditor/live/GenesisLivePackets.hpp"
#include "GenesisLiveShared/GenesisLiveHeader.hpp"

namespace genesis::live
{

    GenesisLiveConnectionPacketBase* GenesisLiveConnectionPacketUtils::sfCreateRelayPacketByType(GenesisLiveConnectionPacketType Type)
    {
        switch (Type)
        {
        case GenesisLiveConnectionPacketType::REQUEST_GENESIS_SERIALIZED_FILE:
            return new GenesisLiveConnectionPacketRequestSerializedFile();
        case GenesisLiveConnectionPacketType::RESPONSE_GENESIS_SERIALIZED_FILE:
            return new GenesisLiveConnectionPacketResponseSerializedFile();
        default:
            return nullptr;
        }
    }

    ash::AshCustomResult<ash::AshBuffer*> GenesisLiveConnectionPacketUtils::sfSerializePacket(std::string Identification, GenesisLiveConnectionPacketBase* Packet)
    {
        ash::AshStreamExpandableExportBuffer bufferStream = ash::AshStreamExpandableExportBuffer();
        GenesisLivePacketHeader packetHeader = GenesisLivePacketHeader(Identification, static_cast<unsigned int>(Packet->GetType()));

        if (packetHeader.Export(&bufferStream) == false)
        {
            return ash::AshCustomResult<ash::AshBuffer*>(false, "Failed to export packet header.");
        }

        if (Packet->Export(&bufferStream) == false)
        {
            return ash::AshCustomResult<ash::AshBuffer*>(false, "Failed to export packet");
        }

        return ash::AshCustomResult<ash::AshBuffer*>(true).AttachResult(bufferStream.MakeCopyOfBuffer());
    }

    ash::AshCustomResult<GenesisLiveConnectionPacketBase*> GenesisLiveConnectionPacketUtils::sfDeserializePacket(std::string Identification, ash::AshBuffer* Buffer)
    {
        ash::AshStreamStaticBuffer stream = ash::AshStreamStaticBuffer(Buffer, ash::AshStreamMode::READ);
        GenesisLivePacketHeader packetHeader = GenesisLivePacketHeader(Identification);

        if (packetHeader.Import(&stream) == false)
        {
            return ash::AshCustomResult<GenesisLiveConnectionPacketBase*>(false, "Failed to import header.");
        }

        if (packetHeader.IsIdentification(Identification) == false)
        {
            return ash::AshCustomResult<GenesisLiveConnectionPacketBase*>(false, "Packet header identification does not match.");
        }

        GenesisLiveConnectionPacketBase* packet = sfCreateRelayPacketByType(packetHeader.GetTypeAs<GenesisLiveConnectionPacketType>());

        if (packet == nullptr)
        {
            return ash::AshCustomResult<GenesisLiveConnectionPacketBase*>(false, "Failed to create packet by type.");
        }

        if (packet->Import(&stream) == false)
        {
            return ash::AshCustomResult<GenesisLiveConnectionPacketBase*>(false, "Failed to deserialize packet.");
        }

        return ash::AshCustomResult<GenesisLiveConnectionPacketBase*>(true).AttachResult(packet);
    }

} // namespace genesis::live