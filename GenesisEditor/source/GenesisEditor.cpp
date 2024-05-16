#include "GenesisEditor/GenesisEditor.hpp"
#include "GenesisRenderer/GenesisRenderer.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <thread>

namespace ed = ax::NodeEditor;

namespace genesis::editor
{

    GenesisEditor::GenesisEditor():
        m_TestBundleEditor()
    {
        m_Renderer = renderer::GenesisRendererProvider::CreateRenderer(1600, 900, "Genesis Editor");
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

        m_TestBundleEditor.Initialize();

        this->ApplyStyle();
        
        while (m_Renderer->ShallRender()) 
        {
            m_Renderer->BeginFrame();
            
            if(ImGui::Begin("GenesisEditorFrame", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
                ImGui::SetWindowPos(ImVec2(0, 0));

                m_TestBundleEditor.Render();
                
                ImGui::End();

            }

            m_Renderer->EndFrame();

            std::this_thread::yield();
        }

        m_TestBundleEditor.Shutdown();

        m_Renderer->Shutdown();
    }

}