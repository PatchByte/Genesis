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
        static bool sfRenderOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation);
        static bool sfRenderFindPatternOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisFindPatternOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderMathOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisMathOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderDebugOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisDebugOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderGetOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisGetOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderRipOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisRipOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderOutputDataClassMemberVariableOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassMemberVariableOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderOutputDataClassVirtualFunctionOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassVirtualFunctionOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderOutputDataClassNonVirtualFunctionOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassNonVirtualFunctionOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderOutputDataStaticFunctionOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataStaticFunctionOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
        static bool sfRenderOutputDataStaticVariableOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataStaticVariableOperation* Operation, const operations::GenesisOperationInformation& OperationInformation);
    };

}

#endif // !_GENESISOPERATIONSEDITOR_HPP