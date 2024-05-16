#ifndef _GENESISBUNDLEEDITOR_HPP
#define _GENESISBUNDLEEDITOR_HPP

#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"

namespace genesis::editor
{

    class GenesisBundleEditor : public GenesisBundle
    {
    public:
        GenesisBundleEditor();

        void Initialize();
        void Shutdown();
        void Render();

        static GenesisFlow* sfDefaultFactory(void* Reserved);

    private:
        bool m_DockSpaceHasBeenBuilt;
        ImGuiID m_DockSpaceId;
        ImGuiID m_DockSidebarWindow;
        ImGuiID m_DockContentWindow;
        ImGuiID m_DockLogWindow;

        utils::GenesisLogBox m_LogBox;

        std::string m_SelectedFlow;
    };

} // namespace genesis::editor

#endif // !_GENESISBUNDLEEDITOR_HPP