#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"

namespace genesis::editor
{

    float GenesisOperationEditorForNodes::sfGetMaxItemWidth(std::string Text)
    {
        static float smNodeMaxTakenWidth = ImGui::CalcTextSize(Text.data()).x;

        return smNodeMaxTakenWidth;
    }

    bool GenesisOperationEditorForNodes::sfRenderOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation)
    {
        operations::GenesisOperationInformation operationInformation = Operation->GetOperationInformation();

        switch (Operation->GetOperationType())
        {
        default:
        case operations::GenesisOperationType::INVALID:
            Builder.End();
            return false;
        case operations::GenesisOperationType::FIND_PATTERN:
            return sfRenderFindPatternOperation(Builder, dynamic_cast<operations::GenesisFindPatternOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::MATH:
            return sfRenderMathOperation(Builder, dynamic_cast<operations::GenesisMathOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::DEBUG:
            return sfRenderDebugOperation(Builder, dynamic_cast<operations::GenesisDebugOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::GET:
            return sfRenderGetOperation(Builder, dynamic_cast<operations::GenesisGetOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::RIP:
            return sfRenderRipOperation(Builder, dynamic_cast<operations::GenesisRipOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::OUTPUT_DATA_CLASS_MEMBER_VARIABLE:
            return sfRenderOutputDataClassMemberVariableOperation(Builder, dynamic_cast<operations::GenesisOutputDataClassMemberVariableOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::OUTPUT_DATA_CLASS_VIRTUAL_FUNCTION:
            return sfRenderOutputDataClassVirtualFunctionOperation(Builder, dynamic_cast<operations::GenesisOutputDataClassVirtualFunctionOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::OUTPUT_DATA_CLASS_NON_VIRTUAL_FUNCTION:
            return sfRenderOutputDataClassNonVirtualFunctionOperation(Builder, dynamic_cast<operations::GenesisOutputDataClassNonVirtualFunctionOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::OUTPUT_DATA_STATIC_FUNCTION:
            return sfRenderOutputDataStaticFunctionOperation(Builder, dynamic_cast<operations::GenesisOutputDataStaticFunctionOperation*>(Operation), operationInformation);
        case operations::GenesisOperationType::OUTPUT_DATA_STATIC_VARIABLE:
            return sfRenderOutputDataStaticVariableOperation(Builder, dynamic_cast<operations::GenesisOutputDataStaticVariableOperation*>(Operation), operationInformation);
        }
    }

}