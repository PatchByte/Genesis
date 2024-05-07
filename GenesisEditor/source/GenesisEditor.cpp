#include "GenesisEditor/GenesisEditor.hpp"
#include "GenesisRenderer/GenesisRenderer.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"
#include "imnodes.h"
#include <thread>

namespace genesis::editor
{

    GenesisEditor::GenesisEditor():
        m_TestFlowEditor()
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

        m_TestFlowEditor.Initialize();

        this->ApplyStyle();
        
        while (m_Renderer->ShallRender()) 
        {
            m_Renderer->BeginFrame();
            
            if(ImGui::Begin("GenesisEditorFrame", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
                ImGui::SetWindowPos(ImVec2(0, 0));

                m_TestFlowEditor.Render();
                
                ImGui::End();

            }

            m_Renderer->EndFrame();

            std::this_thread::yield();
        }

        ImNodes::DestroyContext();
        m_Renderer->Shutdown();
    }

}