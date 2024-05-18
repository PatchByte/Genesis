#include "GenesisShared/GenesisFlow.hpp"
#include "Ash/AshResult.h"
#include "AshObjects/AshBufferPlug.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "GenesisShared/GenesisPinTracker.hpp"
#include "GenesisShared/GenesisState.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

namespace genesis
{

    GenesisFlow::GenesisFlow():
        m_Operations(),
        m_CounterLinks(0),
        m_CounterOperations(1),
        m_Links(),
        m_ReservedBufferGui()
    {}

    GenesisFlow::~GenesisFlow()
    {
        this->Reset();
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
                    // printf("Removing Link %lx:%lx\n", Value.first, Value.second);
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

            std::pair<uintptr_t, uintptr_t> value = currentIterator.second;


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

    GenesisOperationState* GenesisFlow::CreateOperationState(common::GenesisLoadedFile* LoadedFile)
    {
        GenesisOperationState* state = new GenesisOperationState(LoadedFile);
        return state;
    }

    ash::AshResult GenesisFlow::ProcessFlow(output::GenesisOutputData* OutputData, common::GenesisLoadedFile* LoadedFile)
    {
        // Traversing setup

        std::vector<operations::GenesisBaseOperation*> flowStartNodes = CollectAllFlowStarterNodes(true, true);
        std::function<ash::AshResult(operations::GenesisBaseOperation*, std::vector<operations::GenesisOperationId>, GenesisOperationState* CurrentOperationState)> traverseNode;
        
        // Traversing function

        traverseNode = [this, &traverseNode] (operations::GenesisBaseOperation* Operation, std::vector<operations::GenesisOperationId> PreviousVisitedOperationsIds, GenesisOperationState* CurrentOperationState) -> ash::AshResult 
        {
            // Check

            if(std::find(PreviousVisitedOperationsIds.begin(), PreviousVisitedOperationsIds.end(), Operation->GetOperationId()) != PreviousVisitedOperationsIds.end())
            {
                return ash::AshResult(false, fmt::format("Node {} has already been visited. Loop detected ;)", Operation->GetOperationId()));
            }

            PreviousVisitedOperationsIds.push_back(Operation->GetOperationId());

            // Node Processing

            if(auto res = Operation->ProcessOperation(CurrentOperationState); res.HasError())
            {
                return ash::AshResult(false, fmt::format("Node {} failed. {}", Operation->GetOperationId(), res.GetMessage()));
            }

            // Link traversal

            auto linksVector = CollectAllNodeLinkIdsToOtherNodesFromNode(Operation->GetOperationId());

            for(operations::GenesisOperationId nextLinkNode : linksVector)
            {
                GenesisOperationState* nextOperationState = new GenesisOperationState(*CurrentOperationState);

                if(auto res = traverseNode(m_Operations.at(nextLinkNode), PreviousVisitedOperationsIds, nextOperationState); res.HasError())
                {
                    return res;
                }

                delete nextOperationState;
            }

            return ash::AshResult(true);
        };

        for(operations::GenesisBaseOperation* currentFlowStartNode : flowStartNodes)
        {
            GenesisOperationState* currentFlowStartNodeOperationState = this->CreateOperationState(LoadedFile);
            currentFlowStartNodeOperationState->SetOutputData(OutputData);

            auto res = traverseNode(currentFlowStartNode, {}, currentFlowStartNodeOperationState);
            
            // Do not need that anymore.
            delete currentFlowStartNodeOperationState;

            if(res.HasError())
            {
                return ash::AshResult(false, fmt::format("(Start node: {}) {}", currentFlowStartNode->GetOperationId(), res.GetMessage()));
            }

        }
        
        return ash::AshResult(true);
    }

    void GenesisFlow::Reset()
    {
        m_CounterOperations = 1;
        m_CounterLinks = 0;

        for(auto currentIterator : m_Operations)
        {
            delete currentIterator.second;
        }

        m_Operations.clear();
        m_Links.clear();

        m_ReservedBufferGui.ReleaseMemory();
    }

    bool GenesisFlow::Import(ash::AshStream* Stream)
    {   
        this->Reset();

        m_CounterOperations = Stream->Read<operations::GenesisOperationId>();
        m_CounterLinks = Stream->Read<int>();

        {
            size_t operationsSize = Stream->Read<size_t>();

            for(size_t currentOperationIndex = 0; currentOperationIndex < operationsSize; currentOperationIndex++)
            {
                operations::GenesisOperationId currentOperationId = Stream->Read<operations::GenesisOperationId>();
                operations::GenesisBaseOperation* currentOperation = operations::GenesisOperationUtils::sfCreateOperationByType(static_cast<operations::GenesisOperationType>(Stream->Read<int>()));

                if(currentOperation == nullptr)
                {
                    std::cout << "Failed. currentOperation is nullptr." << std::endl;
                    std::exit(-1);
                }

                if(currentOperation->Import(Stream) == false)
                {
                    std::cout << "Failed to import currentOperation" << std::endl;
                    std::exit(-1);
                    return false;
                }

                currentOperation->SetOperationId(currentOperationId);

                m_Operations.emplace(currentOperationId, currentOperation);
            }
        }

        {
            size_t linksSize = Stream->Read<size_t>();

            for(size_t currentLinkIndex = 0; currentLinkIndex < linksSize; currentLinkIndex++)
            {
                uintptr_t currentLinkKey = Stream->Read<uintptr_t>();

                uintptr_t currentLinkValueLeft = Stream->Read<uintptr_t>();
                uintptr_t currentLinkValueRight = Stream->Read<uintptr_t>();

                m_Links.emplace(currentLinkKey, std::make_pair(currentLinkValueLeft, currentLinkValueRight));
            }
        }

        ash::objects::AshDataBufferPlug reservedBufferGuiPlug = ash::objects::AshDataBufferPlug();

        if(reservedBufferGuiPlug.Import(Stream) == false)
        {
            std::cout << "Failed to import reservedBufferGuiPlug." << std::endl;
            return false;
        }

        if(auto res = reservedBufferGuiPlug.GetBuffer(); res)
        {
            m_ReservedBufferGui.CopyAshBufferFromPointer(res);
            delete res;
        }
        return Stream->IsOkay();
    }

    bool GenesisFlow::Export(ash::AshStream* Stream)
    {
        Stream->Write<operations::GenesisOperationId>(m_CounterOperations);
        Stream->Write<int>(m_CounterLinks);

        Stream->Write<size_t>(m_Operations.size());

        for(auto currentOperationIterator : m_Operations)
        {
            Stream->Write<operations::GenesisOperationId>(currentOperationIterator.first);
            Stream->Write<int>(static_cast<int>(currentOperationIterator.second->GetOperationType()));

            if(currentOperationIterator.second->Export(Stream) == false)
            {
                std::cout << "Failed to export currentOperationIterator." << std::endl;
                return false;
            }
        }

        Stream->Write<size_t>(m_Links.size());

        for(auto currentLinkIterator : m_Links)
        {
            Stream->Write<uintptr_t>(currentLinkIterator.first);
            Stream->Write<uintptr_t>(currentLinkIterator.second.first);
            Stream->Write<uintptr_t>(currentLinkIterator.second.second);
        }

        ash::objects::AshDataBufferPlug(&m_ReservedBufferGui).Export(Stream);

        return Stream->IsOkay();
    }

}