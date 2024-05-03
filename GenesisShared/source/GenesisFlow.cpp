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
        for(operations::GenesisBaseOperation* currentOperation : m_Operations)
        {
            delete currentOperation;
        }

        m_Operations.clear();
    }

    void GenesisFlow::ForeachObject(sdForeachIteratorDelegate Delegate)
    {
        stOperationsVector copiedOperationsVector = m_Operations;
        GenesisFlowIndex index = 0;

        for(operations::GenesisBaseOperation* currentOperation : copiedOperationsVector)
        {
            if(Delegate(index, currentOperation) == false) break;

            index++;
        }
    }

    GenesisFlowIndex GenesisFlow::AddOperationToFlow(operations::GenesisBaseOperation* Operation)
    {
        GenesisFlowIndex index = m_Operations.size();
        m_Operations.push_back(Operation);

        return index;
    }

    bool GenesisFlow::RemoveOperationFromFlow(GenesisFlowIndex Index)
    {
        if(Index >= m_Operations.size())
        {
            return false;
        }

        operations::GenesisBaseOperation* operation = m_Operations.at(Index);
        m_Operations.erase(m_Operations.begin() + Index);
        delete operation;

        return true;
    }

    ash::AshCustomResult<unsigned long long> GenesisFlow::ProcessFlow(common::GenesisLoadedFile* LoadedFile)
    {
        operations::GenesisOperationState state = operations::GenesisOperationState(LoadedFile);

        for(operations::GenesisBaseOperation* currentOperation : m_Operations)
        {
            if(auto res = currentOperation->ProcessOperation(&state); res.HasError())
            {
                return ash::AshCustomResult<unsigned long long>(false, "State failed. " + res.GetMessage());
            }
        }

        return state.GetRawValue();
    }

    bool GenesisFlow::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString flowNameString = ash::objects::AshAsciiString();

        if(flowNameString.Import(Stream) == false)
        {
            return false;
        }

        m_Name = flowNameString.GetText();

        return m_Operations.Import(Stream);
    }

    bool GenesisFlow::Export(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString(m_Name).Export(Stream);

        return m_Operations.Export(Stream);
    }

}