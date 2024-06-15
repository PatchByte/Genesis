#include "Ash/AshBuffer.h"
#include "AshObjects/AshString.h"
#include "GenesisEditor/live/GenesisLivePackets.hpp"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::live
{

    GenesisLiveConnectionPacketOperationUpdate::GenesisLiveConnectionPacketOperationUpdate() : m_FlowName(), m_OperationId(-1), m_Buffer()
    {
    }

    GenesisLiveConnectionPacketOperationUpdate::GenesisLiveConnectionPacketOperationUpdate(std::string FlowName, operations::GenesisOperationId OperationId, ash::AshBuffer Buffer)
        : m_FlowName(FlowName), m_OperationId(OperationId), m_Buffer(Buffer)
    {
    }

    bool GenesisLiveConnectionPacketOperationUpdate::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString flowNameObject = ash::objects::AshAsciiString();

        m_OperationId = Stream->Read<operations::GenesisOperationId>();

        if (flowNameObject.Import(Stream))
        {
            m_FlowName = flowNameObject.GetText();
        }

        ash::AshSize bufferSize = Stream->Read<ash::AshSize>();

        m_Buffer.ReleaseMemory();

        if (bufferSize > 0)
        {
            m_Buffer.AllocateSize(bufferSize);
            Stream->ReadRawIntoPointer(m_Buffer.GetPointer(), m_Buffer.GetSize());
        }

        return true;
    }

    bool GenesisLiveConnectionPacketOperationUpdate::Export(ash::AshStream* Stream)
    {
        Stream->Write<operations::GenesisOperationId>(m_OperationId);

        ash::objects::AshAsciiString(m_FlowName).Export(Stream);

        Stream->Write<ash::AshSize>(m_Buffer.GetSize());
        Stream->WriteRawFromPointer(m_Buffer.GetPointer(), m_Buffer.GetSize());

        return true;
    }

} // namespace genesis::live