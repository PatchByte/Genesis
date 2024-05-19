#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "fmt/format.h"

namespace genesis::operations
{

    static constexpr ash::AshSize smGenesisOutputDataClassVTableFunctionOperationHeader = 0x474E535343564620; 

    GenesisOutputDataClassVTableFunctionOperation::GenesisOutputDataClassVTableFunctionOperation():
        m_ClassName(),
        m_FunctionName()
    {}

    GenesisOutputDataClassVTableFunctionOperation::GenesisOutputDataClassVTableFunctionOperation(std::string ClassName, std::string FunctionName):
        m_ClassName(ClassName),
        m_FunctionName(FunctionName)
    {}

    GenesisOperationInformation GenesisOutputDataClassVTableFunctionOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = false;
        operationInformation.m_IsOutputOperation = true;

        return std::move(operationInformation);
    }

    ash::AshResult GenesisOutputDataClassVTableFunctionOperation::ProcessOperation(GenesisOperationState* State)
    {
        State->GetOutputData()->GetOrCreateClass(m_ClassName)->AddVirtualFunctions(output::GenesisOutputClassVirtualFunction(m_FunctionName, State->GetRawValue()));

        if(State->GetRawValue() % 8)
        {
            return ash::AshResult(false, fmt::format("Offset is not valid. 0x{:x}", State->GetRawValue()));
        }

        return ash::AshResult(true);
    }

    bool GenesisOutputDataClassVTableFunctionOperation::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString classNameObject = ash::objects::AshAsciiString(), functionNameObject = ash::objects::AshAsciiString();

        if(Stream->Read<ash::AshSize>() != smGenesisOutputDataClassVTableFunctionOperationHeader)
        {
            return false;
        }

        bool res = classNameObject.Import(Stream);
        res &= functionNameObject.Import(Stream);

        if(res)
        {
            m_ClassName = classNameObject.GetText();
            m_FunctionName = functionNameObject.GetText();
        }

        return Stream->IsOkay() && res;
    }

    bool GenesisOutputDataClassVTableFunctionOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(smGenesisOutputDataClassVTableFunctionOperationHeader);

        ash::objects::AshAsciiString(m_ClassName).Export(Stream);
        ash::objects::AshAsciiString(m_FunctionName).Export(Stream);

        return Stream->IsOkay();
    }
    
}