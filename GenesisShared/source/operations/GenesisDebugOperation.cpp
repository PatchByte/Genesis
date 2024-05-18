#include "Ash/AshBuffer.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::operations
{
    static constexpr ash::AshSize smGenesisDebugOperationHeader = 0x474E535344454247; 

    GenesisDebugOperation::GenesisDebugOperation():
        m_DebugMessage()
    {}

    GenesisDebugOperation::GenesisDebugOperation(std::string DebugMessage):
        m_DebugMessage(DebugMessage)
    {}

    GenesisOperationInformation GenesisDebugOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = false;

        return std::move(operationInformation);
    }

    ash::AshResult GenesisDebugOperation::ProcessOperation(GenesisOperationState* State)
    {
        if(auto debugLogger = State->GetLogger())
        {
            debugLogger->Log("Debug", "Node {} \"{}\" 0x{:x}.", m_OperationId, m_DebugMessage, State->GetRawValue());
        }

        return ash::AshResult(true);
    }

    bool GenesisDebugOperation::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString debugMessageObject = ash::objects::AshAsciiString();

        if(Stream->Read<ash::AshSize>() != smGenesisDebugOperationHeader)
        {
            return false;
        }

        if(debugMessageObject.Import(Stream) == false)
        {
            return false;
        }

        m_DebugMessage = debugMessageObject.GetText();

        return Stream->IsOkay();
    }

    bool GenesisDebugOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(smGenesisDebugOperationHeader);
        
        ash::objects::AshAsciiString(m_DebugMessage).Export(Stream);

        return Stream->IsOkay();
    }

}