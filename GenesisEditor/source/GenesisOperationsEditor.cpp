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

    void GenesisOperationEditorForNodes::sfRenderOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation)
    {
        operations::GenesisOperationInformation operationInformation = Operation->GetOperationInformation();

        switch (Operation->GetOperationType())
        {
        default:
        case operations::GenesisOperationType::INVALID:
            Builder.End();
            return;
        case operations::GenesisOperationType::FIND_PATTERN:
            sfRenderFindPatternOperation(Builder, dynamic_cast<operations::GenesisFindPatternOperation*>(Operation), operationInformation);
            return;
        case operations::GenesisOperationType::MATH:
            sfRenderMathOperation(Builder, dynamic_cast<operations::GenesisMathOperation*>(Operation), operationInformation);
            break;
        case operations::GenesisOperationType::DEBUG:
            sfRenderDebugOperation(Builder, dynamic_cast<operations::GenesisDebugOperation*>(Operation), operationInformation);
            break;
        case operations::GenesisOperationType::GET:
            sfRenderGetOperation(Builder, dynamic_cast<operations::GenesisGetOperation*>(Operation), operationInformation);
            break;
        }
    }

}