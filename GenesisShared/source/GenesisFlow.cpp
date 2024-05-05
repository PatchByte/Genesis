#include "GenesisShared/GenesisFlow.hpp"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis
{

    GenesisFlow::GenesisFlow():
        m_Name(),
        m_Operations()
    {}

    GenesisFlow::~GenesisFlow()
    {
        
    }

    operations::GenesisOperationId GenesisFlow::AddOperationToFlow(operations::GenesisBaseOperation* Operation)
    {
        
        return 0;
    }

    bool GenesisFlow::RemoveOperationFromFlow(operations::GenesisOperationId Index)
    {
        return false;
    }

    ash::AshCustomResult<unsigned long long> GenesisFlow::ProcessFlow(common::GenesisLoadedFile* LoadedFile)
    {
        
        exit(-1);
    }

    bool GenesisFlow::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString flowNameString = ash::objects::AshAsciiString();

        if(flowNameString.Import(Stream) == false)
        {
            return false;
        }

        m_Name = flowNameString.GetText();

        return false;
    }

    bool GenesisFlow::Export(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString(m_Name).Export(Stream);

        return false;
    }

}