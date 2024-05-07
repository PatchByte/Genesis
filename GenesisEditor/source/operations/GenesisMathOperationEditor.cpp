#include "GenesisEditor/GenesisOperationsEditor.hpp"
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

    void GenesisOperationEditorForNodes::sfRenderMathOperation(operations::GenesisMathOperation* Operation, const operations::GenesisOperationInformation& OperationInformation)
    {
        static const char* typeItems[] = {
            "Nothing (Invalid)",
            "Addition",
            "Subtraction"
        };

        operations::GenesisMathOperation::Type type = Operation->GetType();
        unsigned long long value = Operation->GetValue();

        //ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("Subtraction Combo"));
        //if(ImGui::Combo("##Type", reinterpret_cast<int*>(&type), typeItems, sizeof(typeItems) / sizeof(*typeItems)))
        //{
        //    Operation->SetType(type);
        //}
        //if(ImGui::InputScalar("##Value", ImGuiDataType_U32, &value, 0, 0, "%d"))
        //{
        //    Operation->SetValue(value);
        //}
        //ImGui::PopItemWidth();
        
        ed::BeginPin(OperationInformation.m_InputPinId, ax::NodeEditor::PinKind::Input);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("input"));
        ImGui::Text("Input %li", OperationInformation.m_InputPinId);
        ImGui::PopItemWidth();
        ed::EndPin();

        ed::BeginPin(OperationInformation.m_OutputPinId, ax::NodeEditor::PinKind::Output);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("output"));
        ImGui::Text("Output %li", OperationInformation.m_OutputPinId);
        ImGui::PopItemWidth();
        ed::EndPin();
    }

} // namespace genesis::editor