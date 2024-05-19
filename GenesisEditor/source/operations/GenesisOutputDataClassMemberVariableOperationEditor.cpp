#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"

namespace genesis::editor
{

    void GenesisOperationEditorForNodes::sfRenderOutputDataClassMemberVariableOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassMemberVariableOperation* Operation,
                                                                                        const operations::GenesisOperationInformation& OperationInformation)
    {
        std::string className = Operation->GetClassName();
        std::string memberName = Operation->GetMemberName();
        std::string typeDefinition = Operation->GetTypeDefinition();

        Builder.Middle();

        ImGui::PushItemWidth(300);

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Class Name", &className))
        {
            Operation->SetClassName(className);
        }

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Member Name", &memberName))
        {
            Operation->SetMemberName(memberName);
        }

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Type Definition", &typeDefinition))
        {
            Operation->SetTypeDefinition(typeDefinition);
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