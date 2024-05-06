#include "Ash/AshBuffer.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"
#include "Ash/AshResult.h"
#include "mem/mem.h"
#include "mem/pattern.h"

namespace genesis::operations
{

    static constexpr ash::AshSize smGenesisFindPatternOperationHeader = 0x474e535346494e44;

    GenesisFindPatternOperation::GenesisFindPatternOperation():
        m_Pattern()
    {}

    GenesisFindPatternOperation::GenesisFindPatternOperation(std::string Pattern):
        m_Pattern(Pattern)
    {}

    GenesisOperationInformation GenesisFindPatternOperation::GetOperationInformation()
    {
        GenesisOperationInformation information;

        information.m_IsFlowStartNode = true;
        information.m_DiscardsPreviousValue = true;

        return std::move(information);
    }

    ash::AshResult GenesisFindPatternOperation::ProcessOperation(GenesisOperationState* State)
    {
        mem::pattern parsedPattern = mem::pattern(m_Pattern.c_str());

        auto patternResult = mem::scan(parsedPattern, mem::region(
            mem::pointer(State->GetLoadedFile()->GetTargetBuffer().GetPointer()),
            State->GetLoadedFile()->GetTargetBuffer().GetSize()
        ));

        if(patternResult.any() == 0)
        {
            return ash::AshResult(false, "Could not find pattern!");
        }

        if(auto translateResult = State->GetLoadedFile()->TranslateBufferAddressToVirtualAddress(patternResult.any()); translateResult == -1)
        {
            return ash::AshResult(false, "Could not translate result!");
        }
        else 
        {
            State->SetRawValue(translateResult);
        }

        return true;
    }

    bool GenesisFindPatternOperation::Import(ash::AshStream* Stream)
    {
        if(Stream->Read<ash::AshSize>() != smGenesisFindPatternOperationHeader)
        {
            return false;
        }

        ash::objects::AshAsciiString patternString = ash::objects::AshAsciiString();

        if(patternString.Import(Stream) == false)
        {
            return false;
        }

        m_Pattern = patternString.GetText();

        return Stream->IsOkay();
    }

    bool GenesisFindPatternOperation::Export(ash::AshStream* Stream)
    {
        Stream->Write(smGenesisFindPatternOperationHeader);
        ash::objects::AshAsciiString(m_Pattern).Export(Stream);

        return Stream->IsOkay();
    }


}