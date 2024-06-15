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

    bool GenesisOperationEditorForNodes::sfRenderFindPatternOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisFindPatternOperation* Operation,
                                                                      const operations::GenesisOperationInformation& OperationInformation)
    {
        bool changed = false;

        std::string patternBuffer = Operation->GetPattern();

        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("Pattern Input Bla Bla"));
        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("##Pattern", &patternBuffer, ImVec2(190, 0)))
        {
            changed |= true;
            Operation->SetPattern(patternBuffer);
        }

        ImGui::PopItemWidth();

        Builder.Output(OperationInformation.m_OutputPinId);
        ImGui::Text("Output");
        Builder.EndOutput();

        Builder.End();

        return changed;
    }

} // namespace genesis::editor