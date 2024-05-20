#ifndef _GENESISBUNDLEEDITOR_HPP
#define _GENESISBUNDLEEDITOR_HPP

#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"

namespace genesis::editor
{

    class GenesisBundleEditor : public GenesisBundle
    {
    public:
        GenesisBundleEditor(utils::GenesisLogBox* LogBox);
        ~GenesisBundleEditor();

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

        static GenesisFlow* sfDefaultFactory(void* Reserved);

    private:
        bool m_DockSpaceHasBeenBuilt;
        ImGuiID m_DockSpaceId;
        ImGuiID m_DockSidebarWindow;
        ImGuiID m_DockContentWindow;
        ImGuiID m_DockLogWindow;

        utils::GenesisLogBox* m_LogBox;

        std::string m_SelectedFlow;
        std::string m_SearchBoxName;

        ImFont* m_KeyboardFont;
    };

} // namespace genesis::editor

#endif // !_GENESISBUNDLEEDITOR_HPP