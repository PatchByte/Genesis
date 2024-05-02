#include "GenesisEditor/GenesisEditor.hpp"
#include "GenesisRenderer/GenesisRenderer.hpp"
#include <thread>

namespace genesis::editor
{

    GenesisEditor::GenesisEditor()
    {
        m_Renderer = renderer::GenesisRendererProvider::CreateRenderer(800, 450, "Genesis Editor");
    }

    GenesisEditor::~GenesisEditor()
    {
        if(m_Renderer)
        {
            delete m_Renderer;
        }
    }

    void GenesisEditor::Run()
    {
        m_Renderer->Initialize();
        
        while (m_Renderer->ShallRender()) 
        {
            m_Renderer->BeginFrame();
            m_Renderer->EndFrame();

            std::this_thread::yield();
        }

        m_Renderer->Shutdown();
    }

}