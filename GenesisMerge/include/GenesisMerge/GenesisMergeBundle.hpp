#ifndef _GENESISMERGEBUNDLE_HPP
#define _GENESISMERGEBUNDLE_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "AshLogger/AshLogger.h"
#include "GenesisMerge/GenesisMergeFlow.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include <filesystem>
#include <map>

namespace genesis::merge
{

    class GenesisBundleMerge
    {
    public:
        using sdFlows = std::map<std::string, GenesisFlowMerge*>;

        GenesisBundleMerge(ash::AshLogger& Logger, GenesisBundle* Base, GenesisBundle* Local, GenesisBundle* Remote, std::filesystem::path Merged);
        ~GenesisBundleMerge();

        ash::AshResult IsMergeable();
        ash::AshCustomResult<ash::AshBuffer*> Serialize();

        void Render();

        static GenesisFlow* sfFactory(void* Reserved);

    private:
        ash::AshLogger& m_Logger;
        sdFlows m_Flows;
        std::filesystem::path m_Merged;
    };

} // namespace genesis::merge

#endif // !_GENESISMERGEBUNDLE_HPP
