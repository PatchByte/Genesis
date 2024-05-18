#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"

namespace genesis::editor
{

    void GenesisOperationEditorForNodes::sfRenderDebugOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisDebugOperation* Operation, const operations::GenesisOperationInformation& OperationInformation)
    {
        std::string debugMessage = Operation->GetDebugMessage();
        
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("Pattern Input Bla Bla"));
        if(widgets::GenesisGenericWidgets::sfRenderInputTextStlString("##Debug Message", &debugMessage, ImVec2(190, 0)))
        {
            Operation->SetDebugMessage(debugMessage);    
        }
        ImGui::PopItemWidth();

        Builder.Input(OperationInformation.m_OutputPinId);
        ImGui::Text("Input");
        Builder.EndInput();

        Builder.End();
    }

}