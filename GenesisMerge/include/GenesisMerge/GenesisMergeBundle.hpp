#ifndef _GENESISMERGEBUNDLE_HPP
#define _GENESISMERGEBUNDLE_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshLogger/AshLogger.h"
#include "GenesisMerge/GenesisMergeFlow.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include <map>

namespace genesis::merge
{

    class GenesisBundleMerge
    {
    public:
        using sdFlows = std::map<std::string, GenesisFlowMerge*>;

        GenesisBundleMerge(ash::AshLogger& Logger, GenesisBundle* Base, GenesisBundle* Local, GenesisBundle* Remote);
        ~GenesisBundleMerge();

        ash::AshResult IsMergeable();
        ash::AshCustomResult<ash::AshBuffer*> Serialize();

        void Render();

        static GenesisFlow* sfFactory(void* Reserved);

    private:
        ash::AshLogger& m_Logger;
        sdFlows m_Flows;
    };

} // namespace genesis::merge

#endif // !_GENESISMERGEBUNDLE_HPP
