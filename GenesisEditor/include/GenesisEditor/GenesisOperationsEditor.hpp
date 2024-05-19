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
        static void sfRenderFindPatternOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisFindPatternOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static void sfRenderMathOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisMathOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static void sfRenderDebugOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisDebugOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static void sfRenderGetOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisGetOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static void sfRenderRipOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisRipOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static void sfRenderOutputDataClassMemberVariableOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassMemberVariableOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static void sfRenderOutputDataClassVirtualFunctionOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassVirtualFunctionOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static void sfRenderOutputDataClassNonVirtualFunctionOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassNonVirtualFunctionOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
    };

}

#endif // !_GENESISOPERATIONSEDITOR_HPP