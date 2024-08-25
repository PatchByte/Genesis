#include "GenesisMerge/GenesisMerge.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshStream.h"
#include "AshLogger/AshLoggerPassage.h"
#include "AshLogger/AshLoggerTag.h"
#include "GenesisMerge/GenesisMergeBundle.hpp"
#include "GenesisMerge/GenesisMergeGit.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "fmt/chrono.h"
#include "fmt/format.h"
#include "imgui.h"
#include <filesystem>
#include <map>
#include <string_view>

namespace genesis::merge
{
    static constexpr std::string_view smBaseFontPath = "resources/Cantarell-Regular.ttf";

    GenesisMerge::GenesisMerge() : m_Logger("GenesisMerge", {}), m_Renderer(nullptr), m_MergedPath(), m_BasePath(), m_LocalPath(), m_RemotePath(), m_FinalExitCode(GenesisGitExitCodes::ABORTED)
    {
        m_Renderer = renderer::GenesisRendererProvider::CreateRenderer(500, 600, "Genesis Merge");
        m_Logger.AddLoggerPassage(
            new ash::AshLoggerFunctionPassage([](ash::AshLoggerDefaultPassage* This, ash::AshLoggerTag Tag, std::string Format, fmt::format_args Args, std::string FormattedString)
                                              { std::cout << This->GetParent()->GetPrefixFunction()(Tag, Format, Args) << " " << FormattedString << std::endl; }));

        m_Logger.SetPrefixFunction([](ash::AshLoggerTag LoggerTag, std::string LoggerFormat, fmt::format_args LoggerFormatArgs) -> std::string
                                   { return fmt::format("[{}/{}/{}]", "Merge", LoggerTag.GetPrefix(), std::chrono::system_clock::now()); });
    }

    GenesisGitExitCodes GenesisMerge::Run(int ArgCount, const char** ArgArray)
    {
        if (ArgCount < 5)
        {
            m_Logger.Log("Error", "Invalid arguments. GenesisMerge [Base] [Local] [Remote] [Merged]");
            return GenesisGitExitCodes::ABORTED;
        }

        m_BasePath = ArgArray[1];
        m_LocalPath = ArgArray[2];
        m_RemotePath = ArgArray[3];
        m_MergedPath = ArgArray[4];

        m_BaseBundle = new GenesisBundle(GenesisBundleMerge::sfFactory);
        m_LocalBundle = new GenesisBundle(GenesisBundleMerge::sfFactory);
        m_RemoteBundle = new GenesisBundle(GenesisBundleMerge::sfFactory);

        for (auto currentIterator : std::map<std::filesystem::path, GenesisBundle*>{{m_BasePath, m_BaseBundle}, {m_LocalPath, m_LocalBundle}, {m_RemotePath, m_RemoteBundle}})
        {
            ash::AshBuffer currentBuffer = ash::AshBuffer();

            if (currentBuffer.ReadFromFile(currentIterator.first).HasError())
            {
                m_Logger.Log("Error", "Failed to read file. {}", currentIterator.first.string());
                return GenesisGitExitCodes::ABORTED;
            }

            ash::AshStreamStaticBuffer stream = ash::AshStreamStaticBuffer(&currentBuffer, ash::AshStreamMode::READ);

            if (currentIterator.second->Import(&stream) == false)
            {
                m_Logger.Log("Error", "Failed to import file. {}", currentIterator.first.string());
                return GenesisGitExitCodes::ABORTED;
            }
        }

        m_BundleMerge = new GenesisBundleMerge(m_Logger, m_BaseBundle, m_LocalBundle, m_RemoteBundle);

        m_Logger.Log("Info", "Loaded files, all set.");

        // Gui initialization

        m_Renderer->Initialize();

        // Post Gui initialization

        if (std::filesystem::exists(smBaseFontPath))
        {
            m_DefaultFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(smBaseFontPath.data(), 22.);
            ImGui::GetIO().FontDefault = m_DefaultFont;
        }
        else
        {
            m_Logger.Log("Warning", "Fonts are not present, please add them.");
        }

        while (m_Renderer->ShallRender())
        {
            m_Renderer->BeginFrame();

            this->Render();

            m_Renderer->EndFrame();
        }

        m_Renderer->Shutdown();

        delete m_Renderer;
        m_Renderer = nullptr;

        return m_FinalExitCode;
    }

    void GenesisMerge::Render()
    {
        if (ImGui::Begin("GenesisMergeFrame", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_MenuBar*/))
        {
            ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::SetWindowPos(ImVec2(0, 0));

            m_BundleMerge->Render();
        }
        ImGui::End();
    }

} // namespace genesis::merge
