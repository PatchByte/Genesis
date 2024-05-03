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
#include <string>
#include <vector>

namespace genesis
{
    using GenesisFlowIndex = size_t;

    class GenesisFlow : public ash::AshStreamableObject
    {
    public:
        using stOperationsVector = ash::objects::AshPointerVector<operations::GenesisBaseOperation>;
        using sdForeachIteratorDelegate = std::function<bool(GenesisFlowIndex Index, operations::GenesisBaseOperation* Operation)>;
        
        GenesisFlow();
        ~GenesisFlow();

        std::string GetFlowName() { return m_Name; }
        
        void ForeachObject(sdForeachIteratorDelegate Delegate);
        GenesisFlowIndex AddOperationToFlow(operations::GenesisBaseOperation* Operation);
        bool RemoveOperationFromFlow(GenesisFlowIndex Index);

        ash::AshCustomResult<unsigned long long> ProcessFlow(common::GenesisLoadedFile* LoadedFile);

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);
    protected:
        std::string m_Name;
        stOperationsVector m_Operations;
    };

}

#endif // !_GENESISFLOW_HPP