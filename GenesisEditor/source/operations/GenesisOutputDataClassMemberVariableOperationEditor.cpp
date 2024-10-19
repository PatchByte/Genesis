#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"

namespace genesis::editor
{

    bool GenesisOperationEditorForNodes::sfRenderOutputDataClassMemberVariableOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisOutputDataClassMemberVariableOperation* Operation,
                                                                                        const operations::GenesisOperationInformation& OperationInformation)
    {
        bool changed = false;

        std::string className = Operation->GetClassName();
        std::string memberName = Operation->GetMemberName();

        Builder.Middle();

        ImGui::PushItemWidth(300);

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Class Name", &className))
        {
            changed |= true;
            Operation->SetClassName(className);
        }

        if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Member Name", &memberName))
        {
            changed |= true;
            Operation->SetMemberName(memberName);
        }

        //! @deprecated
        // if (widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Type Definition", &typeDefinition))
        // {
        //     changed |= true;
        //     Operation->SetTypeDefinition(typeDefinition);
        // }

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
