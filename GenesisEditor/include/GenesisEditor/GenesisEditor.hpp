#ifndef _GENESISEDITOR_HPP
#define _GENESISEDITOR_HPP

#include "GenesisEditor/GenesisFlowEditor.hpp"
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
        renderer::GenesisRendererBase* m_Renderer;
        GenesisFlowEditor m_TestFlowEditor;
    };

}

#endif // !_GENESISEDITOR_HPP