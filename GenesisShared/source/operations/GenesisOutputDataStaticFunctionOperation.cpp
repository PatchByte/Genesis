#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "fmt/format.h"

namespace genesis::operations
{

    static constexpr ash::AshSize smGenesisOutputDataStaticFunctionOperationHeader = 0x474E535353462020; 

    GenesisOutputDataStaticFunctionOperation::GenesisOutputDataStaticFunctionOperation():
        m_ClassName(),
        m_FunctionName()
    {}

    GenesisOutputDataStaticFunctionOperation::GenesisOutputDataStaticFunctionOperation(std::string FunctionName):
        m_ClassName(),
        m_FunctionName(FunctionName)
    {}

    GenesisOutputDataStaticFunctionOperation::GenesisOutputDataStaticFunctionOperation(std::string ClassName, std::string FunctionName):
        m_ClassName(ClassName),
        m_FunctionName(FunctionName)
    {}

    GenesisOperationInformation GenesisOutputDataStaticFunctionOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = false;
        operationInformation.m_IsOutputOperation = true;

        return std::move(operationInformation);
    }

    ash::AshResult GenesisOutputDataStaticFunctionOperation::ProcessOperation(GenesisOperationState* State)
    {
        if(State->GetLoadedFile()->TranslateVirtualAddressToFileOffset(State->GetRawValue()) == -1)
        {
            return ash::AshResult(false, fmt::format("Invalid address in image. ({:x})", State->GetRawValue()));
        }

        common::Address globalOffset = State->GetRawValue() - State->GetLoadedFile()->GetNtHeaders()->OptionalHeader.ImageBase;

        State->GetOutputData()->GetOrCreateClass(m_ClassName)->AddStaticFunction(output::GenesisOutputStaticFunction(m_FunctionName, globalOffset)); 

        return ash::AshResult(true);
    }

    bool GenesisOutputDataStaticFunctionOperation::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString classNameObject = ash::objects::AshAsciiString(), functionNameObject = ash::objects::AshAsciiString();

        if(Stream->Read<ash::AshSize>() != smGenesisOutputDataStaticFunctionOperationHeader)
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

    bool GenesisOutputDataStaticFunctionOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(smGenesisOutputDataStaticFunctionOperationHeader);

        ash::objects::AshAsciiString(m_ClassName).Export(Stream);
        ash::objects::AshAsciiString(m_FunctionName).Export(Stream);

        return Stream->IsOkay();
    }
    
}