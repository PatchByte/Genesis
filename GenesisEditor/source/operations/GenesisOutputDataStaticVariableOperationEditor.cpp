#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"

namespace genesis::editor
{

    void GenesisOperationEditorForNodes::sfRenderOutputDataStaticVariableOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataStaticVariableOperation* Operation,
                                                                                        const operations::GenesisOperationInformation& OperationInformation)
    {
        std::string className = Operation->GetClassName();
        std::string variableName = Operation->GetVariableName();

        Builder.Middle();

        ImGui::PushItemWidth(300);

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Class Name", &className))
        {
            Operation->SetClassName(className);
        }

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Variable Name", &variableName))
        {
            Operation->SetVariableName(variableName);
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