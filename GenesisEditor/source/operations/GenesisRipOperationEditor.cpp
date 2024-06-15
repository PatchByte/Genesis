#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"

namespace genesis::editor
{

    bool GenesisOperationEditorForNodes::sfRenderRipOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisRipOperation* Operation,
                                                               const operations::GenesisOperationInformation& OperationInformation)
    {
        bool changed = false;
        unsigned char carry = Operation->GetCarry();

        Builder.Middle();

        ImGui::PushItemWidth(200);

        if(ImGui::InputScalar("##Carry", ImGuiDataType_U8, &carry))
        {
            changed |= true;
            Operation->SetCarry(carry);
        }

        ImGui::PopItemWidth();

        Builder.Input(OperationInformation.m_InputPinId);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("input"));
        ImGui::Text("Input");
        ImGui::PopItemWidth();
        Builder.EndInput();

        Builder.Output(OperationInformation.m_OutputPinId);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("output"));
        ImGui::Text("Output");
        ImGui::PopItemWidth();
        Builder.EndOutput();

        // Because of manual post actions
        Builder.End();

        return changed;
    }

}