#ifndef _GENESISEDITOR_HPP
#define _GENESISEDITOR_HPP

#include "GenesisRenderer/GenesisRenderer.hpp"
namespace genesis::editor
{

    class GenesisEditor
    {
    public:
        GenesisEditor();
        ~GenesisEditor();

        void Run();
    private:
        renderer::GenesisRendererBase* m_Renderer;
    };

}

#endif // !_GENESISEDITOR_HPP