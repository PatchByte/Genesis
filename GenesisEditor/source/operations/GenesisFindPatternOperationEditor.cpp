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

    void GenesisOperationEditorForNodes::sfRenderFindPatternOperation(operations::GenesisFindPatternOperation* Operation, const operations::GenesisOperationInformation& OperationInformation)
    {
        std::string patternBuffer = Operation->GetPattern();

        ImNodes::BeginStaticAttribute(OperationInformation.m_InputPinId);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("Pattern Input Bla Bla"));
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

        ImNodes::BeginOutputAttribute(OperationInformation.m_OutputPinId);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("output"));
        ImGui::TextUnformatted("Output");
        ImGui::PopItemWidth();
        ImNodes::EndOutputAttribute();
    }

} // namespace genesis::editor