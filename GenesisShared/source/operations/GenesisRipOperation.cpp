#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::operations
{

    static constexpr ash::AshSize smGenesisRipOperationHeader = 0x474E535352495020;

    GenesisRipOperation::GenesisRipOperation() : m_Carry(4)
    {
    }

    GenesisRipOperation::GenesisRipOperation(unsigned char Carry) : m_Carry(Carry)
    {
    }

    GenesisOperationInformation GenesisRipOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_IsInteractOperation = true;
        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = true;

        return operationInformation;
    }

    ash::AshResult GenesisRipOperation::ProcessOperation(GenesisOperationState* State)
    {
        common::Address translatedAddressToOffsetInBuffer = State->GetLoadedFile()->TranslateVirtualAddressToFileOffset(State->GetRawValue());

        if (translatedAddressToOffsetInBuffer == -1)
        {
            return ash::AshResult(false, "Invalid address as input.");
        }

        signed long long nextIpSigned = State->GetRawValue();

        nextIpSigned += *State->GetLoadedFile()->GetTargetBuffer().GetBufferAtOffset<signed int>(State->GetLoadedFile()->TranslateVirtualAddressToFileOffset(State->GetRawValue()));
        nextIpSigned += m_Carry;

        State->SetRawValue(nextIpSigned);

        if(State->GetLoadedFile()->TranslateVirtualAddressToFileOffset(nextIpSigned) == -1)
        {
            return ash::AshResult(false, "Failed to rip.");
        }

        return ash::AshResult(true);
    }

    bool GenesisRipOperation::Import(ash::AshStream* Stream)
    {
        if (Stream->Read<ash::AshSize>() != smGenesisRipOperationHeader)
        {
            return false;
        }

        m_Carry = Stream->Read<unsigned char>();

        return Stream->IsOkay();
    }

    bool GenesisRipOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(smGenesisRipOperationHeader);
        Stream->Write<unsigned char>(m_Carry);

        return Stream->IsOkay();
    }

} // namespace genesis::operations