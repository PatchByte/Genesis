#ifndef _GENESISFLOW_HPP
#define _GENESISFLOW_HPP

#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "Ash/AshStreamableObject.h"
#include "AshObjects/AshPointerVector.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "GenesisShared/GenesisState.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace genesis
{
    class GenesisFlow : public ash::AshStreamableObject
    {
    public:
        using sdFlowOperationsMap = std::map<operations::GenesisOperationId, operations::GenesisBaseOperation*>;
        using sdFlowLinks = std::map<uintptr_t, std::pair<uintptr_t, uintptr_t>>;
        using sdOperationsVector = std::vector<operations::GenesisBaseOperation*>;
        using sdOperationIdsVector = std::vector<operations::GenesisOperationId>;

        GenesisFlow();
        ~GenesisFlow();

        virtual GenesisOperationState* CreateOperationState(common::GenesisLoadedFile* LoadedFile);
        ash::AshResult ProcessFlow(output::GenesisOutputData* OutputData, common::GenesisLoadedFile* LoadedFile);

        operations::GenesisOperationId FindFreeOperationId();
        std::pair<operations::GenesisBaseOperation*, operations::GenesisOperationId> CreateOperationInFlowFromType(operations::GenesisOperationType OperationType);
        bool RemoveOperationFromFlow(operations::GenesisOperationId OperationId);

        sdOperationIdsVector CollectAllNodeLinkIdsToOtherNodesFromNode(operations::GenesisOperationId OperationId);
        sdOperationsVector CollectAllFlowStarterNodes(bool IncludeNonConditionalNodes = true, bool IncludeConditionalNodes = false);

        ash::AshResult CheckIfFlowIsRunnable();

        virtual void Reset();
        bool Import(ash::AshStream* Stream) override;
        bool Export(ash::AshStream* Stream) override;

    protected:
        operations::GenesisOperationId m_CounterOperations;
        int m_CounterLinks;

        sdFlowOperationsMap m_Operations;
        sdFlowLinks m_Links;

        // This is a reserved buffer for the GUI implementation.
        ash::AshBuffer m_ReservedBufferGui;
    };

} // namespace genesis

#endif // !_GENESISFLOW_HPP