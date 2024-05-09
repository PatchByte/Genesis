#ifndef _GENESISFLOWEDITOR_HPP
#define _GENESISFLOWEDITOR_HPP

#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/GenesisNodeBuilder.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"

#include "imgui.h"
#include "imgui_node_editor.h"
#include <map>

namespace genesis::editor
{

    class GenesisFlowEditor : public GenesisFlow
    {
    public:
        GenesisFlowEditor();
        ~GenesisFlowEditor();

        void Initialize();
        void Shutdown();
        void Render();
        void RenderNodes();
        void RenderNodeOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation);
    
        static bool sfGetColorForOperationInformation(const operations::GenesisOperationInformation& Information, ImColor* OutputNormalColor, ImColor* OutputBrightColor);

        void Reset() override;
        bool Import(ash::AshStream* Stream) override;
        bool Export(ash::AshStream* Stream) override;
    private:
        ash::AshLogger m_Logger;
        utils::GenesisLogBox m_LogBox;

        bool m_DockSpaceHasBeenBuilt;
        ImGuiID m_DockSpaceId;
        ImGuiID m_DockNodeWindow;
        ImGuiID m_DockLogWindow;

        ax::NodeEditor::EditorContext* m_NodeEditorContext;
        std::map<uintptr_t, std::string> m_NodeEditorSavedStates;

        bool m_TriggerCheck;

    };

}

#endif // !_GENESISFLOWEDITOR_HPP