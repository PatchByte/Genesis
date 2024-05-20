#ifndef _GENESISFLOWEDITOR_HPP
#define _GENESISFLOWEDITOR_HPP

#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/GenesisNodeBuilder.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"

#include "imgui.h"
#include "imgui_canvas.h"
#include "imgui_node_editor.h"
#include <map>
#include <utility>

namespace genesis::editor
{

    class GenesisFlowEditor : public GenesisFlow
    {
    public:
        enum class ActionType
        {
            NAVIGATE_TO_CONTENT
        };

        GenesisFlowEditor(utils::GenesisLogBox* LogBox);
        ~GenesisFlowEditor();

        void Initialize();
        void Shutdown();
        void Render(std::string UniqueKey);
        void RenderNodes(std::string UniqueKey);
        void RenderNodeOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation);

        static bool sfGetColorForOperationInformation(const operations::GenesisOperationInformation& Information, ImColor* OutputNormalColor);

        GenesisOperationState* CreateOperationState(common::GenesisLoadedFile* LoadedFile) override;

        void Reset() override;
        bool Import(ash::AshStream* Stream) override;
        bool Export(ash::AshStream* Stream) override;

        void DoAction(ActionType Action, void* Reserved);

    private:
        ash::AshLogger m_Logger;
        utils::GenesisLogBox* m_LogBox;

        ax::NodeEditor::EditorContext* m_NodeEditorContext;

        std::map<uintptr_t, std::pair<float, float>> m_NodeEditorSavedStates;

        bool m_TriggerCheck;
        bool m_TriggerActionFocusFirstNode;
        bool m_TriggerRestoreStateOfNodes;
    };

} // namespace genesis::editor

#endif // !_GENESISFLOWEDITOR_HPP