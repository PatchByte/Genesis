#include "Ash/AshResult.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::operations
{

    static constexpr ash::AshSize smGenesisGetOperationHeader = 0x474E535347455420; 

    GenesisGetOperation::GenesisGetOperation():
        m_ValueKind(ValueKindType::UINT8)
    {}

    GenesisGetOperation::GenesisGetOperation(GenesisGetOperation::ValueKindType ValueKind):
        m_ValueKind(ValueKind)
    {}

    GenesisOperationInformation GenesisGetOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_IsInteractOperation = true;
        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = true;

        return operationInformation;
    }

    ash::AshResult GenesisGetOperation::ProcessOperation(GenesisOperationState* State)
    {
        common::Address translatedAddressToOffsetInBuffer = State->GetLoadedFile()->TranslateVirtualAddressToFileOffset(State->GetRawValue());

        if(translatedAddressToOffsetInBuffer == -1)
        {
            return ash::AshResult(false, "Invalid address as input.");
        }

        unsigned long long value = 0;

        switch (m_ValueKind) 
        {
        case ValueKindType::UINT8:
            value = *State->GetLoadedFile()->GetTargetBuffer().GetBufferAtOffset<uint8_t>(translatedAddressToOffsetInBuffer);
            break;
        case ValueKindType::UINT16:
            value = *State->GetLoadedFile()->GetTargetBuffer().GetBufferAtOffset<uint16_t>(translatedAddressToOffsetInBuffer);
            break;
        case ValueKindType::UINT32:
            value = *State->GetLoadedFile()->GetTargetBuffer().GetBufferAtOffset<uint32_t>(translatedAddressToOffsetInBuffer);
            break;
        case ValueKindType::UINT64:
            value = *State->GetLoadedFile()->GetTargetBuffer().GetBufferAtOffset<uint64_t>(translatedAddressToOffsetInBuffer);
        default:
            return ash::AshResult(false, "Unhandled value kind type.");
        }

        State->SetRawValue(value);

        return ash::AshResult(true);
    }

    bool GenesisGetOperation::Import(ash::AshStream* Stream)
    {
        if(Stream->Read<ash::AshSize>() != smGenesisGetOperationHeader)
        {
            return false;
        }

        m_ValueKind = static_cast<GenesisGetOperation::ValueKindType>(Stream->Read<int>());
        
        return Stream->IsOkay();
    }

    bool GenesisGetOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write(smGenesisGetOperationHeader);
        Stream->Write(static_cast<int>(m_ValueKind));

        return Stream->IsOkay();
    }

}