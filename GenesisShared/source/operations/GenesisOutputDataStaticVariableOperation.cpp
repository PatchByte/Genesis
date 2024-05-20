#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "fmt/format.h"

namespace genesis::operations
{

    static constexpr ash::AshSize smGenesisOutputDataStaticVariableOperationHeader = 0x474E535353562020; 

    GenesisOutputDataStaticVariableOperation::GenesisOutputDataStaticVariableOperation():
        m_ClassName(),
        m_VariableName()
    {}

    GenesisOutputDataStaticVariableOperation::GenesisOutputDataStaticVariableOperation(std::string VariableName):
        m_ClassName(),
        m_VariableName(VariableName)
    {}

    GenesisOutputDataStaticVariableOperation::GenesisOutputDataStaticVariableOperation(std::string ClassName, std::string VariableName):
        m_ClassName(ClassName),
        m_VariableName(VariableName)
    {}

    GenesisOperationInformation GenesisOutputDataStaticVariableOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = false;
        operationInformation.m_IsOutputOperation = true;

        return std::move(operationInformation);
    }

    ash::AshResult GenesisOutputDataStaticVariableOperation::ProcessOperation(GenesisOperationState* State)
    {
        if(State->GetLoadedFile()->TranslateVirtualAddressToFileOffset(State->GetRawValue()) == -1)
        {
            return ash::AshResult(false, fmt::format("Invalid address in image. ({:x})", State->GetRawValue()));
        }

        common::Address globalOffset = State->GetRawValue() - State->GetLoadedFile()->GetNtHeaders()->OptionalHeader.ImageBase;

        State->GetOutputData()->GetOrCreateClass(m_ClassName)->AddStaticVariable(output::GenesisOutputStaticVariable(m_VariableName, globalOffset)); 

        return ash::AshResult(true);
    }

    bool GenesisOutputDataStaticVariableOperation::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString classNameObject = ash::objects::AshAsciiString(), variableNameObject = ash::objects::AshAsciiString();

        if(Stream->Read<ash::AshSize>() != smGenesisOutputDataStaticVariableOperationHeader)
        {
            return false;
        }

        bool res = classNameObject.Import(Stream);
        res &= variableNameObject.Import(Stream);

        if(res)
        {
            m_ClassName = classNameObject.GetText();
            m_VariableName = variableNameObject.GetText();
        }

        return Stream->IsOkay() && res;
    }

    bool GenesisOutputDataStaticVariableOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(smGenesisOutputDataStaticVariableOperationHeader);

        ash::objects::AshAsciiString(m_ClassName).Export(Stream);
        ash::objects::AshAsciiString(m_VariableName).Export(Stream);

        return Stream->IsOkay();
    }
    
}