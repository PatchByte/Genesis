#ifndef _GENESISEDITOR_HPP
#define _GENESISEDITOR_HPP

#include "Ash/AshResult.h"
#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisBundleEditor.hpp"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisEditor/live/GenesisLive.hpp"
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

        void Run(int ArgCount, const char** ArgArray);

        void ApplyStyle();

        ash::AshResult LoadGenesisFileFrom(std::filesystem::path Path);
        ash::AshResult SaveGenesisFileTo(std::filesystem::path Path);
        ash::AshCustomResult<std::string> ProcessGenesisFile(std::filesystem::path Input);
        ash::AshResult LoadGenesisFileFromAndApplyLogs(std::filesystem::path Path);
        ash::AshResult SaveGenesisFileToAndApplyLogs(std::filesystem::path Path);
        ash::AshCustomResult<std::string> ProcessGenesisFileAndApplyLogs(std::filesystem::path Input);

    private:
        utils::GenesisLogBox m_LogBox;
        ash::AshLogger m_Logger;
        bool m_ForceDisableRendering;

        renderer::GenesisRendererBase* m_Renderer;
        GenesisBundleEditor m_BundleEditor;

        ImFont* m_DefaultFont;
        ImFont* m_KeyboardFont;

        // QOL
        std::filesystem::path m_LastProcessedInputFile;
        std::filesystem::path m_LastProcessedOutputFile;

        std::filesystem::path m_LastUsedFile;

        // Live
        live::GenesisLive* m_Live;
    };

} // namespace genesis::editor

#endif // !_GENESISEDITOR_HPP