#include "Ash/AshBuffer.h"
#include "GenesisEditor/live/GenesisLivePackets.hpp"

namespace genesis::live
{

    GenesisLiveConnectionPacketResponseSerializedFile::GenesisLiveConnectionPacketResponseSerializedFile():
        m_SerializedFile()
    {}

    bool GenesisLiveConnectionPacketResponseSerializedFile::Import(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(m_SerializedFile.GetSize());
        Stream->WriteRawFromPointer(m_SerializedFile.GetPointer(), m_SerializedFile.GetSize());

        return Stream->IsOkay();
    }

    bool GenesisLiveConnectionPacketResponseSerializedFile::Export(ash::AshStream* Stream)
    {
        ash::AshSize serializedFileSize = Stream->Read<ash::AshSize>();

        if(serializedFileSize > GenesisLiveConnectionPacketResponseSerializedFile::smBufferMaxSize)
        {
            return false;
        }

        if(m_SerializedFile.AllocateSize(serializedFileSize) == false)
        {
            return false;
        }

        Stream->ReadRawIntoPointer(m_SerializedFile.GetPointer(), m_SerializedFile.GetSize());

        return Stream->IsOkay();
    }

}