#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "fmt/format.h"

namespace genesis::operations
{

    static constexpr ash::AshSize smGenesisOutputDataClassNonVirtualFunctionOperationHeader = 0x474E5353434E5646; 

    GenesisOutputDataClassNonVirtualFunctionOperation::GenesisOutputDataClassNonVirtualFunctionOperation():
        m_ClassName(),
        m_FunctionName()
    {}

    GenesisOutputDataClassNonVirtualFunctionOperation::GenesisOutputDataClassNonVirtualFunctionOperation(std::string ClassName, std::string FunctionName):
        m_ClassName(ClassName),
        m_FunctionName(FunctionName)
    {}

    GenesisOperationInformation GenesisOutputDataClassNonVirtualFunctionOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = false;
        operationInformation.m_IsOutputOperation = true;

        return std::move(operationInformation);
    }

    ash::AshResult GenesisOutputDataClassNonVirtualFunctionOperation::ProcessOperation(GenesisOperationState* State)
    {
        if(State->GetLoadedFile()->TranslateVirtualAddressToFileOffset(State->GetRawValue()) == -1)
        {
            return ash::AshResult(false, fmt::format("Invalid address in image. ({:x})", State->GetRawValue()));
        }

        common::Address globalOffset = State->GetRawValue() - State->GetLoadedFile()->GetNtHeaders()->OptionalHeader.ImageBase;

        State->GetOutputData()->GetOrCreateClass(m_ClassName)->AddNonVirtualFunction(output::GenesisOutputClassNonVirtualFunction(m_FunctionName, globalOffset)); 


        return ash::AshResult(true);
    }

    bool GenesisOutputDataClassNonVirtualFunctionOperation::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString classNameObject = ash::objects::AshAsciiString(), functionNameObject = ash::objects::AshAsciiString();

        if(Stream->Read<ash::AshSize>() != smGenesisOutputDataClassNonVirtualFunctionOperationHeader)
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

    bool GenesisOutputDataClassNonVirtualFunctionOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(smGenesisOutputDataClassNonVirtualFunctionOperationHeader);

        ash::objects::AshAsciiString(m_ClassName).Export(Stream);
        ash::objects::AshAsciiString(m_FunctionName).Export(Stream);

        return Stream->IsOkay();
    }
    
}