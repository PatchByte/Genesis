#include "GenesisShared/GenesisFlow.hpp"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisPinTracker.hpp"
#include <algorithm>
#include <utility>

namespace genesis
{

    GenesisFlow::GenesisFlow():
        m_Name(),
        m_Operations(),
        m_Counter(0)
    {}

    GenesisFlow::~GenesisFlow()
    {
        for(auto currentIterator : m_Operations)
        {
            delete currentIterator.second;
        }

        m_Operations.clear();
        m_Links.clear();
    }

    operations::GenesisOperationId GenesisFlow::AddOperationToFlow(operations::GenesisBaseOperation* Operation)
    {
        Operation->SetOperationId(m_Counter);
        
        m_Operations.emplace(m_Counter, Operation);
        m_Counter++;

        return Operation->GetOperationId();
    }

    bool GenesisFlow::RemoveOperationFromFlow(operations::GenesisOperationId OperationId)
    {
        while(true)
        {
            auto it = std::find_if(m_Links.begin(), m_Links.end(), [&OperationId](std::pair<int, int> Value) -> bool {
                if(utils::GenesisPinValue(Value.first).m_NodeParentId == OperationId || utils::GenesisPinValue(Value.second).m_NodeParentId == OperationId)
                {
                    printf("Removing Link %x:%x\n", Value.first, Value.second);
                    return true;
                }

                return false;
            });

            if(it == m_Links.end()) break;

            m_Links.erase(it);
        }

        m_Operations.erase(OperationId);

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