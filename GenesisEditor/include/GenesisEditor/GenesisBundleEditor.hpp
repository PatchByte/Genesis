#ifndef _GENESISBUNDLEEDITOR_HPP
#define _GENESISBUNDLEEDITOR_HPP

#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/live/GenesisLive.hpp"
#include "GenesisEditor/live/GenesisLivePackets.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"

namespace genesis::editor
{

    class GenesisBundleEditor : public GenesisBundle
    {
    public:
        GenesisBundleEditor(utils::GenesisLogBox* LogBox, live::GenesisLive* Live = nullptr);
        ~GenesisBundleEditor();

        ash::AshResult CreateFlow(std::string FlowName);
        bool Import(ash::AshStream* Stream);

        void Initialize(ImFont* KeyboardFont);
        void Shutdown();
        void Render();

        inline bool HasFlowSelected()
        {
            return m_Flows.contains(m_SelectedFlow);
        }

        inline GenesisFlowEditor* GetSelectedFlow()
        {
            return dynamic_cast<GenesisFlowEditor*>(m_Flows.at(m_SelectedFlow));
        }

        inline void SetLiveInstance(live::GenesisLive* Live)
        {
            m_Live = Live;
        }

        // Live

        inline ash::AshResult SendLiveAction(live::GenesisLiveConnectionPacketBundleAction Action) { return SendLiveAction(&Action); }
        ash::AshResult SendLiveAction(live::GenesisLiveConnectionPacketBundleAction* Action);
        ash::AshResult HandleLiveAction(live::GenesisLiveConnectionPacketBundleAction* Action);

        static GenesisFlow* sfDefaultFactory(void* Reserved);
    protected:
        ash::AshResult ExtractFlowAndSaveToFile(std::string FlowName, std::filesystem::path Output);
        ash::AshResult ExtractFlowAndSaveToFileAndApplyLogs(std::string FlowName, std::filesystem::path Output);
    private:
        bool m_DockSpaceHasBeenBuilt;
        ImGuiID m_DockSpaceId;
        ImGuiID m_DockSidebarWindow;
        ImGuiID m_DockContentWindow;
        ImGuiID m_DockLogWindow;

        utils::GenesisLogBox* m_LogBox;
        ash::AshLogger m_Logger;

        std::string m_SelectedFlow;
        std::string m_SearchBoxName;

        ImFont* m_KeyboardFont;

        // Live
        live::GenesisLive* m_Live;
    };

} // namespace genesis::editor

#endif // !_GENESISBUNDLEEDITOR_HPP