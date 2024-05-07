#include "GenesisShared/GenesisFlow.hpp"
#include "Ash/AshResult.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisPinTracker.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

namespace genesis
{

    GenesisFlow::GenesisFlow():
        m_Name(),
        m_Operations(),
        m_CounterLinks(1),
        m_CounterOperations(1),
        m_Links()
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
        Operation->SetOperationId(m_CounterOperations);
        m_Operations.emplace(m_CounterOperations, Operation);
        
        m_CounterOperations++;

        return Operation->GetOperationId();
    }

    bool GenesisFlow::RemoveOperationFromFlow(operations::GenesisOperationId OperationId)
    {
        while(true)
        {
            auto it = std::find_if(m_Links.begin(), m_Links.end(), [&OperationId](std::pair<uintptr_t, std::pair<uintptr_t, uintptr_t>> Iterator) -> bool {
                std::pair<uintptr_t, uintptr_t> Value = Iterator.second;

                if(utils::GenesisPinValue(Value.first).m_NodeParentId == OperationId || utils::GenesisPinValue(Value.second).m_NodeParentId == OperationId)
                {
                    printf("Removing Link %lx:%lx\n", Value.first, Value.second);
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

    GenesisFlow::sdOperationIdsVector GenesisFlow::CollectAllNodeLinkIdsToOtherNodesFromNode(operations::GenesisOperationId OperationId)
    {
        GenesisFlow::sdOperationIdsVector result = {};

        for(auto currentIterator : m_Links)
        {
            std::pair<int, int> value = currentIterator.second;

            if(utils::GenesisPinValue(value.first).m_NodeParentId == OperationId && utils::GenesisPinValue(value.second).m_NodeParentId != OperationId)
            {
                result.push_back(utils::GenesisPinValue(value.second).m_NodeParentId);
            }
        }

        return result;
    }

    GenesisFlow::sdOperationsVector GenesisFlow::CollectAllFlowStarterNodes(bool IncludeNonConditionalNodes, bool IncludeConditionalNodes)
    {
        sdOperationsVector result = {};

        for(auto currentIterator : m_Operations)
        {
            const operations::GenesisOperationInformation information = currentIterator.second->GetOperationInformation();

            if(information.m_IsFlowStartNode)
            {
                if(information.m_IsConditionalFlowStartNode)
                {
                    if(IncludeConditionalNodes)
                    {
                        result.push_back(currentIterator.second);
                        continue;
                    }
                }

                if(IncludeNonConditionalNodes)
                {
                    result.push_back(currentIterator.second);
                    continue;
                }
            }
        }

        return result;
    }

    ash::AshResult GenesisFlow::CheckIfFlowIsRunnable()
    {
        // Checking if any loop is present

        std::vector<operations::GenesisBaseOperation*> flowStartNodes = CollectAllFlowStarterNodes(true, true);
        
        std::function<ash::AshResult(operations::GenesisBaseOperation*, std::vector<operations::GenesisOperationId>)> traverseNode;
    
        traverseNode = [this, &traverseNode] (operations::GenesisBaseOperation* Operation, std::vector<operations::GenesisOperationId> PreviousVisitedOperationsIds) -> ash::AshResult 
        {
            if(std::find(PreviousVisitedOperationsIds.begin(), PreviousVisitedOperationsIds.end(), Operation->GetOperationId()) != PreviousVisitedOperationsIds.end())
            {
                return ash::AshResult(false, fmt::format("Node {} has already been visited. Loop detected ;)", Operation->GetOperationId()));
            }

            PreviousVisitedOperationsIds.push_back(Operation->GetOperationId());

            auto linksVector = CollectAllNodeLinkIdsToOtherNodesFromNode(Operation->GetOperationId());

            for(operations::GenesisOperationId nextLinkNode : linksVector)
            {
                if(auto res = traverseNode(m_Operations.at(nextLinkNode), PreviousVisitedOperationsIds); res.HasError())
                {
                    return res;
                }
            }

            return ash::AshResult(true);
        };

        for(operations::GenesisBaseOperation* currentFlowStartNode : flowStartNodes)
        {
            if(auto res = traverseNode(currentFlowStartNode, {}); res.HasError())
            {
                return res;
            }
        }

        return ash::AshResult(true);
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