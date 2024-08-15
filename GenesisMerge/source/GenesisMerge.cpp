#include "GenesisMerge/GenesisMerge.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshStream.h"
#include "AshLogger/AshLoggerPassage.h"
#include "AshLogger/AshLoggerTag.h"

#include "GenesisMerge/GenesisGit.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "fmt/chrono.h"
#include "fmt/format.h"
#include <filesystem>
#include <map>

namespace genesis::merge
{
    GenesisMerge::GenesisMerge() : m_Logger("GenesisMerge", {}), m_Renderer(nullptr), m_MergedPath(), m_BasePath(), m_LocalPath(), m_RemotePath(), m_FinalExitCode(GenesisGitExitCodes::ABORTED)
    {
        m_Renderer = renderer::GenesisRendererProvider::CreateRenderer(1600, 900, "Genesis Merge");
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

        m_BaseBundle = new GenesisBundle();
        m_LocalBundle = new GenesisBundle();
        m_RemoteBundle = new GenesisBundle();

        for (auto currentIterator : std::map<std::filesystem::path, GenesisBundle*>{{m_BasePath, m_BaseBundle}, {m_LocalPath, m_LocalBundle}, {m_RemotePath, m_RemoteBundle}})
        {
            ash::AshBuffer currentBuffer = ash::AshBuffer();

            if (currentBuffer.ReadFromFile(currentIterator.first).HasError())
            {
                m_Logger.Log("Error", "Failed to read file. {}", currentIterator.first);
                return GenesisGitExitCodes::ABORTED;
            }

            ash::AshStreamStaticBuffer stream = ash::AshStreamStaticBuffer(&currentBuffer, ash::AshStreamMode::READ);

            if (currentIterator.second->Import(&stream) == false)
            {
                m_Logger.Log("Error", "Failed to import file. {}", currentIterator.first);
                return GenesisGitExitCodes::ABORTED;
            }
        }

        m_Logger.Log("Info", "Loaded files, all set.");

        // Gui initialization

        m_Renderer->Initialize();

        while (m_Renderer->ShallRender())
        {
            m_Renderer->BeginFrame();
            m_Renderer->EndFrame();
        }

        m_Renderer->Shutdown();

        delete m_Renderer;
        m_Renderer = nullptr;

        return m_FinalExitCode;
    }

} // namespace genesis::merge
