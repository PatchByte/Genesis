#ifndef _GENESISMERGE_HPP
#define _GENESISMERGE_HPP

#include "AshLogger/AshLogger.h"
#include "GenesisMerge/GenesisMergeBundle.hpp"
#include "GenesisMerge/GenesisMergeGit.hpp"
#include "GenesisRenderer/GenesisRenderer.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "imgui.h"
#include <filesystem>

namespace genesis::merge
{

    class GenesisMerge
    {
    public:
        GenesisMerge();

        GenesisGitExitCodes Run(int ArgCount, const char** ArgArray);
        void Render();
    private:
        ash::AshLogger m_Logger;
        renderer::GenesisRendererBase* m_Renderer;

        // Gui

        ImFont* m_DefaultFont;

        /*
         * Git Merging works like this:
         * <Base> The most recent first shared commit
         * <Local> Your local commit file
         * <Remote> The remote commit file
         * <Merged> The final output file that has been merged
         *
         * Please fucking kill me :|
         */

        // Bundle Paths
        std::filesystem::path m_BasePath;
        std::filesystem::path m_LocalPath;
        std::filesystem::path m_RemotePath;
        std::filesystem::path m_MergedPath;

        // Bundles
        GenesisBundle* m_BaseBundle;
        GenesisBundle* m_LocalBundle;
        GenesisBundle* m_RemoteBundle;

        GenesisBundleMerge* m_BundleMerge;

        GenesisGitExitCodes m_FinalExitCode;
    };

} // namespace genesis::merge

#endif // !_GENESISMERGE_HPP
