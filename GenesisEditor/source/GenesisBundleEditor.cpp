#include "GenesisEditor/GenesisBundleEditor.hpp"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstring>

namespace genesis::editor
{

    GenesisBundleEditor::GenesisBundleEditor(utils::GenesisLogBox* LogBox)
        : GenesisBundle(GenesisBundleEditor::sfDefaultFactory), m_DockSpaceHasBeenBuilt(false), m_DockSpaceId(0), m_DockSidebarWindow(0), m_DockContentWindow(0), m_DockLogWindow(0), m_SelectedFlow(), m_LogBox(LogBox)
    {
        m_ReservedFactoryValue = this;
    }

    GenesisBundleEditor::~GenesisBundleEditor()
    {
        this->Shutdown();
    }

    void GenesisBundleEditor::Initialize()
    {
        GenesisFlowEditor* flow1 = new GenesisFlowEditor(m_LogBox);
        GenesisFlowEditor* flow2 = new GenesisFlowEditor(m_LogBox);

        // Debug
        m_Flows.emplace("Test 1", flow1);
        m_Flows.emplace("Test 2", flow2);

        // Initialization

        for(auto currentIterator : m_Flows)
        {
            dynamic_cast<GenesisFlowEditor*>(currentIterator.second)->Initialize();
        }
    }

    void GenesisBundleEditor::Shutdown()
    {
    }

    void GenesisBundleEditor::Render()
    {
        if (m_DockSpaceId == 0)
        {
            m_DockSpaceId = ImGui::GetID("dockSpaceBundleEditor");
        }

        if (m_DockSpaceHasBeenBuilt == false)
        {
            ImGui::DockBuilderRemoveNode(m_DockSpaceId);
            ImGui::DockBuilderAddNode(m_DockSpaceId);

            ImGui::DockBuilderSplitNode(m_DockSpaceId, ImGuiDir_Left, 0.2f, &m_DockSidebarWindow, &m_DockContentWindow);
            ImGui::DockBuilderSplitNode(m_DockContentWindow, ImGuiDir_Up, 0.8f, &m_DockContentWindow, &m_DockLogWindow);

            ImGui::DockBuilderDockWindow("Sidebar", m_DockSidebarWindow);
            ImGui::DockBuilderDockWindow("Content", m_DockContentWindow);
            ImGui::DockBuilderDockWindow("Log", m_DockLogWindow);

            ImGui::DockBuilderFinish(m_DockSpaceId);

            m_DockSpaceHasBeenBuilt = true;
        }

        ImGui::DockSpace(m_DockSpaceId, ImVec2(-1, -1),
                         ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_HiddenTabBar |
                             ImGuiDockNodeFlags_NoUndocking);

        if (ImGui::Begin("Sidebar", nullptr))
        {
            // Copy map because of the "Delete" action.
            auto copiedFlows = m_Flows;

            for (auto currentIterator : copiedFlows)
            {
                ImGui::PushID(currentIterator.first.data());
                if (ImGui::Selectable(currentIterator.first.data(), m_SelectedFlow == currentIterator.first))
                {
                    m_SelectedFlow = currentIterator.first;
                }

                static bool sTriggerRenamePopup = false;

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        this->RemoveFlow(currentIterator.first);
                    }

                    if (ImGui::MenuItem("Rename"))
                    {
                        sTriggerRenamePopup |= true;
                    }

                    ImGui::EndPopup();
                }

                if (sTriggerRenamePopup)
                {
                    ImGui::OpenPopup("RenamePopup");
                }

                if (ImGui::BeginPopup("RenamePopup", ImGuiWindowFlags_AlwaysAutoResize))
                {
                    static char sNameBuffer[512] = {0};

                    if (sTriggerRenamePopup == true)
                    {
                        sTriggerRenamePopup = false;
                        std::strcpy(sNameBuffer, currentIterator.first.data());
                    }

                    ImGui::InputText("Name", sNameBuffer, sizeof(sNameBuffer) - 1);
                    ImGui::SameLine();
                    if (ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Enter))
                    {
                        GenesisFlow* renameTarget = m_Flows.at(currentIterator.first);
                        m_Flows.erase(currentIterator.first);
                        m_Flows.emplace(sNameBuffer, renameTarget);
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();

                    if(ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
                    {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Content"))
        {
            if(auto unCastSelectedFlow = GetFlow(m_SelectedFlow))
            {
                GenesisFlowEditor* selectedFlow = dynamic_cast<GenesisFlowEditor*>(unCastSelectedFlow);

                selectedFlow->Render(m_SelectedFlow);
            }
        }
        ImGui::End();

        if (ImGui::Begin("Log"))
        {
            if(m_LogBox)
            {
                m_LogBox->Render();
            }
        }
        ImGui::End();
    }

    GenesisFlow* GenesisBundleEditor::sfDefaultFactory(void* Reserved)
    {
        GenesisBundleEditor* bundleEditor = reinterpret_cast<GenesisBundleEditor*>(Reserved);
        GenesisFlowEditor* flowEditor = new GenesisFlowEditor(bundleEditor->m_LogBox);

        flowEditor->Initialize();

        return flowEditor;
    }

} // namespace genesis::editor