#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"

namespace genesis::editor
{

    bool GenesisOperationEditorForNodes::sfRenderDebugOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisDebugOperation* Operation, const operations::GenesisOperationInformation& OperationInformation)
    {
        bool changed = false;

        std::string debugMessage = Operation->GetDebugMessage();
        
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("Pattern Input Bla Bla"));
        if(widgets::GenesisGenericWidgets::sfRenderInputTextStlString("##Debug Message", &debugMessage, ImVec2(190, 0)))
        {
            Operation->SetDebugMessage(debugMessage);    
            changed |= true;
        }
        ImGui::PopItemWidth();

        Builder.Input(OperationInformation.m_InputPinId);
        ImGui::Text("Input");
        Builder.EndInput();

        Builder.End();
    
        return changed;
    }

}