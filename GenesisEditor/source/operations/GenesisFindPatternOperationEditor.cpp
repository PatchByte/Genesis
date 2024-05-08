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

    void GenesisOperationEditorForNodes::sfRenderFindPatternOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisFindPatternOperation* Operation, const operations::GenesisOperationInformation& OperationInformation)
    {
        std::string patternBuffer = Operation->GetPattern();
        
        //ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("Pattern Input Bla Bla"));
        //if(ImGui::InputTextEx(
        //    "##Pattern", NULL, patternBuffer.data(), patternBuffer.capacity() + 1, ImVec2(190, 0), ImGuiInputTextFlags_CallbackResize,
        //    [](ImGuiInputTextCallbackData* data) -> int
        //    {
        //        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        //        {
        //            std::string* b = reinterpret_cast<std::string*>(data->UserData);
        //            b->resize(data->BufTextLen);
        //            data->Buf = b->data();
        //        }

        //        return 0;
        //    },
        //    &patternBuffer))
        //{
        //    Operation->SetPattern(patternBuffer);    
        //}

        //ImGui::PopItemWidth();

        Builder.Output(OperationInformation.m_OutputPinId);
        ImGui::Text("Output");
        Builder.EndOutput();
    }

} // namespace genesis::editor