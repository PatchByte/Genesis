#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace ed = ax::NodeEditor;

namespace genesis::editor
{

    void GenesisOperationEditorForNodes::sfRenderMathOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisMathOperation* Operation,
                                                               const operations::GenesisOperationInformation& OperationInformation)
    {
        static const char* typeItems[] = {"Nothing (Invalid)", "Addition", "Subtraction"};

        static widgets::GenesisComboBoxPopupWorkaround comboBox = widgets::GenesisComboBoxPopupWorkaround();

        operations::GenesisMathOperation::Type type = Operation->GetType();
        unsigned long long value = Operation->GetValue();

        Builder.Middle();

        ImGui::PushItemWidth(200);

        comboBox.SetData("##Type", {"Nothing", "Addition", "Subtraction"}, reinterpret_cast<int*>(&type), 1);
        comboBox.RenderButton();

        ImGui::PopItemWidth();
        ImGui::PushItemWidth(200);

        if (ImGui::InputScalar("##Value", ImGuiDataType_U64, &value, 0, 0, "%d"))
        {
            Operation->SetValue(value);
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

        comboBox.RenderPost();

        if (comboBox.HasChanged())
        {
            Operation->SetType(type);
        }
    }

} // namespace genesis::editor