#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisOutput.hpp"

namespace genesis::operations
{
    static constexpr ash::AshSize smGenesisOutputDataClassMemberVariableOperationHeader = 0x474E53534F434D56;

    GenesisOutputDataClassMemberVariableOperation::GenesisOutputDataClassMemberVariableOperation() : m_ClassName(), m_TypeDefinition(), m_MemberName()
    {
    }

    GenesisOutputDataClassMemberVariableOperation::GenesisOutputDataClassMemberVariableOperation(std::string ClassName, std::string MemberName, std::string TypeDefinition)
        : m_ClassName(ClassName), m_MemberName(MemberName), m_TypeDefinition(TypeDefinition)
    {
    }

    GenesisOperationInformation GenesisOutputDataClassMemberVariableOperation::GetOperationInformation()
    {
        GenesisOperationInformation operationInformation = GenesisBaseOperation::GetOperationInformation();

        operationInformation.m_HasInputPin = true;
        operationInformation.m_HasOutputPin = false;
        operationInformation.m_IsOutputOperation = true;

        return operationInformation;
    }

    ash::AshResult GenesisOutputDataClassMemberVariableOperation::ProcessOperation(GenesisOperationState* State)
    {
        State->GetOutputData()->GetOrCreateClass(m_ClassName)->AddMember(output::GenesisOutputClassMember(m_MemberName, m_TypeDefinition, State->GetRawValue()));

        return ash::AshResult(true);
    }

    bool GenesisOutputDataClassMemberVariableOperation::Import(ash::AshStream* Stream)
    {
        if (Stream->Read<ash::AshSize>() != smGenesisOutputDataClassMemberVariableOperationHeader)
        {
            return false;
        }

        ash::objects::AshAsciiString classNameObject = ash::objects::AshAsciiString(), memberNameObject = ash::objects::AshAsciiString(), typeDefinitionObject = ash::objects::AshAsciiString();

        bool res = true;

        res &= classNameObject.Import(Stream);
        res &= memberNameObject.Import(Stream);
        res &= typeDefinitionObject.Import(Stream);

        if(res)
        {
            m_ClassName = classNameObject.GetText();
            m_MemberName = memberNameObject.GetText();
            m_TypeDefinition = typeDefinitionObject.GetText();
        }

        return res && Stream->IsOkay();
    }

    bool GenesisOutputDataClassMemberVariableOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write(smGenesisOutputDataClassMemberVariableOperationHeader);
        
        ash::objects::AshAsciiString(m_ClassName).Export(Stream);
        ash::objects::AshAsciiString(m_MemberName).Export(Stream);
        ash::objects::AshAsciiString(m_TypeDefinition).Export(Stream);

        return Stream->IsOkay();
    }
} // namespace genesis::operations