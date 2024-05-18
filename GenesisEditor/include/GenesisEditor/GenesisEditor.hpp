#ifndef _GENESISEDITOR_HPP
#define _GENESISEDITOR_HPP

#include "AshLogger/AshLogger.h"
#include "GenesisEditor/GenesisBundleEditor.hpp"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisLogBox.hpp"
#include "GenesisRenderer/GenesisRenderer.hpp"

namespace genesis::editor
{

    class GenesisEditor
    {
    public:
        GenesisEditor();
        ~GenesisEditor();

        void Run();

        void ApplyStyle();

    private:
        utils::GenesisLogBox m_LogBox;
        ash::AshLogger m_Logger;
        bool m_ForceDisableRendering;

        renderer::GenesisRendererBase* m_Renderer;
        GenesisBundleEditor m_TestBundleEditor;
    };

} // namespace genesis::editor

#endif // !_GENESISEDITOR_HPP