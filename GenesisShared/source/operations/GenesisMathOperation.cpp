#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"
#include <utility>

namespace genesis::operations
{
    static constexpr ash::AshSize smGenesisMathOperationHeader = 0x474e53534d415448; 

    GenesisMathOperation::GenesisMathOperation():
        m_Type(GenesisMathOperation::Type::INVALID),
        m_Value()
    {}

    GenesisMathOperation::GenesisMathOperation(Type MathType, unsigned long long MathValue):
        m_Type(MathType),
        m_Value(MathValue)
    {}

    GenesisOperationInformation GenesisMathOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_IsMathOperation = true;
        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = true;

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

        m_Type = static_cast<GenesisMathOperation::Type>(Stream->Read<int>());
        m_Value = Stream->Read<unsigned long long>();

        return Stream->IsOkay();
    }

    bool GenesisMathOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write(smGenesisMathOperationHeader);
        Stream->Write(static_cast<int>(m_Type));
        Stream->Write(m_Value);

        return Stream->IsOkay();
    }
}