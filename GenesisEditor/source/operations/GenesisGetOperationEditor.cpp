#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::editor
{

    void GenesisOperationEditorForNodes::sfRenderGetOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisGetOperation* Operation,
                                                               const operations::GenesisOperationInformation& OperationInformation)
    {
        static widgets::GenesisComboBoxPopupWorkaround comboBox = widgets::GenesisComboBoxPopupWorkaround();

        operations::GenesisGetOperation::ValueKindType type = Operation->GetValueKind();

        Builder.Middle();

        ImGui::PushItemWidth(200);

        comboBox.SetData("##Type", {"uint8", "uint16", "uint32", "uint64"}, reinterpret_cast<int*>(&type));
        comboBox.RenderButton();

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

        comboBox.RenderPost();

        if (comboBox.HasChanged())
        {
            Operation->SetValueKind(type);
        }
    }

}