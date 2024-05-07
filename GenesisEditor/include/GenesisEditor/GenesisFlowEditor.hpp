#ifndef _GENESISFLOWEDITOR_HPP
#define _GENESISFLOWEDITOR_HPP

#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"

#include "imgui.h"
#include "imnodes.h"

namespace genesis::editor
{

    class GenesisFlowEditor : public GenesisFlow
    {
    public:
        GenesisFlowEditor();
        ~GenesisFlowEditor();

        void Initialize();
        void Render();
        void RenderNodes();
        void RenderNodeOperation(operations::GenesisBaseOperation* Operation);
    
        static bool sfGetColorForOperationInformation(const operations::GenesisOperationInformation& Information, ImColor* OutputNormalColor, ImColor* OutputBrightColor);
    private:
        ash::AshLogger m_Logger;
        utils::GenesisLogBox m_LogBox;

        bool m_DockSpaceHasBeenBuilt;
        ImGuiID m_DockSpaceId;
        ImGuiID m_DockNodeWindow;
        ImGuiID m_DockLogWindow;

        ImNodesContext* m_Context;

        bool m_TriggerCheck;
    };

}

#endif // !_GENESISFLOWEDITOR_HPP