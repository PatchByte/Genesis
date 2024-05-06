#ifndef _GENESISFLOW_HPP
#define _GENESISFLOW_HPP

#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "Ash/AshStreamableObject.h"
#include "AshObjects/AshPointerVector.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include <cstddef>
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
        using sdFlowLinks = std::map<int, std::pair<int, int>>;
        using sdOperationsVector = std::vector<operations::GenesisBaseOperation*>;
        using sdOperationIdsVector = std::vector<operations::GenesisOperationId>;

        GenesisFlow();
        ~GenesisFlow();

        std::string GetFlowName() { return m_Name; }
        ash::AshCustomResult<unsigned long long> ProcessFlow(common::GenesisLoadedFile* LoadedFile);

        operations::GenesisOperationId AddOperationToFlow(operations::GenesisBaseOperation* Operation);
        bool RemoveOperationFromFlow(operations::GenesisOperationId OperationId);

        sdOperationIdsVector CollectAllNodeLinkIdsToOtherNodesFromNode(operations::GenesisOperationId OperationId);
        sdOperationsVector CollectAllFlowStarterNodes(bool IncludeNonConditionalNodes = true, bool IncludeConditionalNodes = false);

        ash::AshResult CheckIfFlowIsRunnable();

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);
    protected:
        std::string m_Name;
        operations::GenesisOperationId m_CounterOperations;
        int m_CounterLinks;

        sdFlowOperationsMap m_Operations;
        sdFlowLinks m_Links;
    };

}

#endif // !_GENESISFLOW_HPP