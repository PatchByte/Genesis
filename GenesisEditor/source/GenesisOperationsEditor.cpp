#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "imgui.h"

namespace genesis::editor
{

    float GenesisOperationEditorForNodes::sfGetMaxItemWidth(std::string Text)
    {
        static float smNodeMaxTakenWidth = ImGui::CalcTextSize(Text.data()).x;

        return smNodeMaxTakenWidth;
    }

    void GenesisOperationEditorForNodes::sfRenderOperation(operations::GenesisBaseOperation* Operation)
    {
        operations::GenesisOperationInformation operationInformation = Operation->GetOperationInformation();

        switch (Operation->GetOperationType())
        {
        default:
        case operations::GenesisOperationType::INVALID:
            return;
        case operations::GenesisOperationType::FIND_PATTERN:
            sfRenderFindPatternOperation(dynamic_cast<operations::GenesisFindPatternOperation*>(Operation), operationInformation);
            return;
        case operations::GenesisOperationType::MATH:
            sfRenderMathOperation(dynamic_cast<operations::GenesisMathOperation*>(Operation), operationInformation);
            break;
        }
    }

}