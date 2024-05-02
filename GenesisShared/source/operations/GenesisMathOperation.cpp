#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::operations
{
    static constexpr ash::AshSize smGenesisMathOperationHeader = 0x474e53534d415448; 

    GenesisMathOperation::GenesisMathOperation():
        m_Type(GenesisMathOperation::Type::INVALID),
        m_Value()
    {}

    GenesisOperationInformation GenesisMathOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation;

        operationInformation.m_IsMathOperation = true;

        return operationInformation;
    }

    ash::AshResult GenesisMathOperation::ProcessOperation(GenesisOperationState* State)
    {
        switch (m_Type) 
        {
            case Type::ADDITION:
                State->SetRawValue(State->GetRawValue() + m_Value);
                break;
            case Type::SUBTRACTION:
                State->SetRawValue(State->GetRawValue() - m_Value);
                break;
            case Type::INVALID:
            default:
                return ash::AshResult(false, "Invalid math operation type");
        }

        return ash::AshResult(true);
    }

    bool GenesisMathOperation::Import(ash::AshStream* Stream)
    {
        if(Stream->Read<ash::AshSize>() != smGenesisMathOperationHeader)
        {
            return false;
        }

        m_Type = static_cast<GenesisMathOperation::Type>(Stream->Read<unsigned char>());
        m_Value = Stream->Read<unsigned long long>();

        return Stream->IsOkay();
    }

    bool GenesisMathOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write(smGenesisMathOperationHeader);
        Stream->Write(static_cast<unsigned char>(m_Type));
        Stream->Write(m_Value);

        return Stream->IsOkay();
    }

}