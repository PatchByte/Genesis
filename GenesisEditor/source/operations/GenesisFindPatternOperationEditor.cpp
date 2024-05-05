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

    static inline float smGetMaxItemWidth(std::string Item = "a")
    {
        static float smNodeMaxTakenWidth = ImGui::CalcTextSize(Item.data()).x;

        return smNodeMaxTakenWidth;
    }

    void GenesisOperationEditorForNodes::sfRenderOperation(operations::GenesisBaseOperation* Operation)
    {
        switch (Operation->GetOperationType())
        {
        default:
        case operations::GenesisOperationType::INVALID:
            return;
        case operations::GenesisOperationType::FIND_PATTERN:
            sfRenderFindPatternOperation(dynamic_cast<operations::GenesisFindPatternOperation*>(Operation));
            return;
        case operations::GenesisOperationType::MATH:
            sfRenderMathOperation(dynamic_cast<operations::GenesisMathOperation*>(Operation));
            break;
        }
    }

    void GenesisOperationEditorForNodes::sfRenderFindPatternOperation(operations::GenesisFindPatternOperation* Operation)
    {
        std::string patternBuffer = Operation->GetPattern();

        ImNodes::BeginStaticAttribute(1);
        ImGui::PushItemWidth(smGetMaxItemWidth("Pattern Input Bla Bla"));
        if(ImGui::InputTextEx(
            "##Pattern", NULL, patternBuffer.data(), patternBuffer.capacity() + 1, ImVec2(190, 0), ImGuiInputTextFlags_CallbackResize,
            [](ImGuiInputTextCallbackData* data) -> int
            {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    std::string* b = reinterpret_cast<std::string*>(data->UserData);
                    b->resize(data->BufTextLen);
                    data->Buf = b->data();
                }

                return 0;
            },
            &patternBuffer))
        {
            Operation->SetPattern(patternBuffer);    
        }

        ImGui::PopItemWidth();
        ImNodes::EndStaticAttribute();

        ImNodes::BeginOutputAttribute(2);
        ImGui::PushItemWidth(smGetMaxItemWidth("output"));
        ImGui::TextUnformatted("Output");
        ImGui::PopItemWidth();
        ImNodes::EndOutputAttribute();
    }

    void GenesisOperationEditorForNodes::sfRenderMathOperation(operations::GenesisMathOperation* Operation)
    {
        static const char* typeItems[] = {
            "Nothing (Invalid)",
            "Addition",
            "Subtraction"
        };

        operations::GenesisMathOperation::Type type = Operation->GetType();

        ImNodes::BeginStaticAttribute(3);
        ImGui::PushItemWidth(smGetMaxItemWidth("Subtraction Combo"));
        if(ImGui::Combo("##Type", reinterpret_cast<int*>(&type), typeItems, sizeof(typeItems) / sizeof(*typeItems)))
        {
            Operation->SetType(type);
        }
        ImGui::PopItemWidth();
        ImNodes::EndStaticAttribute();

        ImNodes::BeginInputAttribute(4);
        ImGui::PushItemWidth(smGetMaxItemWidth("input"));
        ImGui::TextUnformatted("Input");
        ImGui::PopItemWidth();
        ImNodes::EndInputAttribute();

        
    }

} // namespace genesis::editor