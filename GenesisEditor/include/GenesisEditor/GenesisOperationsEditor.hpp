#ifndef _GENESISOPERATIONSEDITOR_HPP
#define _GENESISOPERATIONSEDITOR_HPP

#include "GenesisEditor/GenesisNodeBuilder.hpp"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::editor 
{

    class GenesisOperationEditorForNodes
    {
    public:
        static float sfGetMaxItemWidth(std::string Text);
        static void sfRenderOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation);
        static void sfRenderFindPatternOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisFindPatternOperation* Operation, const operations::GenesisOperationInformation& OperationInfo);
        static void sfRenderMathOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisMathOperation* Operation, const operations::GenesisOperationInformation& OperationInfo);
    };

}

#endif // !_GENESISOPERATIONSEDITOR_HPP