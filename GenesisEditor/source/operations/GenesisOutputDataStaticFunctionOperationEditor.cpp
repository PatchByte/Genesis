#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"

namespace genesis::editor
{

    void GenesisOperationEditorForNodes::sfRenderOutputDataStaticFunctionOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataStaticFunctionOperation* Operation,
                                                                                        const operations::GenesisOperationInformation& OperationInformation)
    {
        std::string className = Operation->GetClassName();
        std::string functionName = Operation->GetFunctionName();

        Builder.Middle();

        ImGui::PushItemWidth(300);

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Class Name", &className))
        {
            Operation->SetClassName(className);
        }

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Function Name", &functionName))
        {
            Operation->SetFunctionName(functionName);
        }

        ImGui::PopItemWidth();

        Builder.Input(OperationInformation.m_InputPinId);
        ImGui::PushItemWidth(GenesisOperationEditorForNodes::sfGetMaxItemWidth("input"));
        ImGui::Text("Input");
        ImGui::PopItemWidth();
        Builder.EndInput();

        // Because of manual post actions
        Builder.End();
    }

} // namespace genesis::editor