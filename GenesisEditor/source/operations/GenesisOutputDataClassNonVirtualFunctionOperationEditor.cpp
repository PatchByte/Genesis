#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"

namespace genesis::editor
{

    bool GenesisOperationEditorForNodes::sfRenderOutputDataClassNonVirtualFunctionOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassNonVirtualFunctionOperation* Operation,
                                                                                        const operations::GenesisOperationInformation& OperationInformation)
    {
        bool changed = false;
        std::string className = Operation->GetClassName();
        std::string functionName = Operation->GetFunctionName();

        Builder.Middle();

        ImGui::PushItemWidth(300);

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Class Name", &className))
        {
            changed |= true;
            Operation->SetClassName(className);
        }

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Function Name", &functionName))
        {
            changed |= true;
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

        return changed;
    }

} // namespace genesis::editor