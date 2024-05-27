#include "Ash/AshBuffer.h"
#include "Ash/AshCRC32.h"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"
#include <cstdint>

namespace genesis::live
{

    static inline ash::AshCRC32Value sfGetBufferCrcValue(ash::AshBuffer& Buffer)
    {
        ash::AshCRC32 crc = ash::AshCRC32();

        crc.UpdateWithTemplateValue(0x726C617942756672);
        crc.Update(Buffer);

        return crc.GetValue();
    }

    GenesisLiveRelayPacketBroadcast::GenesisLiveRelayPacketBroadcast() : m_Sender(-1), m_Buffer()
    {
    }

    bool GenesisLiveRelayPacketBroadcast::Reset()
    {
        m_Buffer.ReleaseMemory();

        return true;
    }

    bool GenesisLiveRelayPacketBroadcast::Import(ash::AshStream* Stream)
    {
        m_Sender = Stream->Read<GenesisPeerId>();

        ash::AshSize bufferSize = Stream->Read<ash::AshSize>();
        ash::AshCRC32Value bufferExpectedCrc = Stream->Read<ash::AshCRC32Value>();

        if (bufferSize > GenesisLiveRelayPacketBroadcast::smBufferMaxSize)
        {
            return false;
        }

        if (m_Buffer.AllocateSize(bufferSize) == false)
        {
            return false;
        }

        Stream->ReadRawIntoPointer(m_Buffer.GetPointer(), bufferSize);

        if (sfGetBufferCrcValue(m_Buffer) != bufferExpectedCrc)
        {
            return false;
        }

        return Stream->IsOkay();
    }

    bool GenesisLiveRelayPacketBroadcast::Export(ash::AshStream* Stream)
    {
        Stream->Write<GenesisPeerId>(m_Sender);
        Stream->Write<ash::AshSize>(m_Buffer.GetSize());
        Stream->Write<ash::AshCRC32Value>(sfGetBufferCrcValue(m_Buffer));
        Stream->WriteRawFromPointer(m_Buffer.GetPointer(), m_Buffer.GetSize());

        return Stream->IsOkay();
    }

} // namespace genesis::live