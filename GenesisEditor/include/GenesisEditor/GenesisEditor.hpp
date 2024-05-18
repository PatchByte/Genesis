#ifndef _GENESISEDITOR_HPP
#define _GENESISEDITOR_HPP

#include "Ash/AshResult.h"
#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisBundleEditor.hpp"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisRenderer/GenesisRenderer.hpp"
#include "imgui.h"
#include <filesystem>

namespace genesis::editor
{

    class GenesisEditor
    {
    public:
        GenesisEditor();
        ~GenesisEditor();

        void Run();

        void ApplyStyle();

        ash::AshResult LoadGenesisFileFrom(std::filesystem::path Path);
        ash::AshResult SaveGenesisFileTo(std::filesystem::path Path);
    private:
        utils::GenesisLogBox m_LogBox;
        ash::AshLogger m_Logger;
        bool m_ForceDisableRendering;

        renderer::GenesisRendererBase* m_Renderer;
        GenesisBundleEditor m_TestBundleEditor;

        ImFont* m_DefaultFont;
        ImFont* m_KeyboardFont;
    };

} // namespace genesis::editor

#endif // !_GENESISEDITOR_HPP