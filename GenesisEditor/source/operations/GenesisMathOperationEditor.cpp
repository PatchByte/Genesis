#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>

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

        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("Subtraction Combo"));
        if(ImGui::Combo("##Type", reinterpret_cast<int*>(&type), typeItems, sizeof(typeItems) / sizeof(*typeItems)))
        {
            Operation->SetType(type);
        }
        if(ImGui::InputScalar("##Value", ImGuiDataType_U32, &value, 0, 0, "%d"))
        {
            Operation->SetValue(value);
        }
        ImGui::PopItemWidth();
        
        ImNodes::BeginInputAttribute(OperationInformation.m_InputPinId);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("input"));
        ImGui::TextUnformatted("Input");
        ImGui::PopItemWidth();
        ImNodes::EndInputAttribute();

        ImNodes::BeginOutputAttribute(OperationInformation.m_OutputPinId);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("output"));
        ImGui::TextUnformatted("Output");
        ImGui::PopItemWidth();
        ImNodes::EndOutputAttribute();
    }

} // namespace genesis::editor