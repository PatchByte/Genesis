#include "GenesisEditor/GenesisBundleEditor.hpp"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstring>

namespace genesis::editor
{

    GenesisBundleEditor::GenesisBundleEditor() : GenesisBundle(GenesisBundleEditor::sfDefaultFactory)
    {
    }

    void GenesisBundleEditor::Initialize()
    {
        GenesisFlowEditor* flow1 = new GenesisFlowEditor();
        GenesisFlowEditor* flow2 = new GenesisFlowEditor();

        // Debug
        m_Flows.emplace("Test 1", flow1);
        m_Flows.emplace("Test 2", flow2);
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

            ImGui::DockBuilderSplitNode(m_DockSpaceId, ImGuiDir_Left, 0.3f, &m_DockSidebarWindow, &m_DockContentWindow);
            ImGui::DockBuilderDockWindow("Sidebar", m_DockSidebarWindow);
            ImGui::DockBuilderDockWindow("Content", m_DockContentWindow);

            ImGui::DockBuilderFinish(m_DockSpaceId);

            m_DockSpaceHasBeenBuilt = true;
        }

        ImGui::DockSpace(m_DockSpaceId, ImVec2(-1, -1),
                         ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_HiddenTabBar);

        if (ImGui::Begin("Sidebar"))
        {
            // Copy map because of the "Delete" action.
            auto copiedFlows = m_Flows;

            for (auto currentIterator : copiedFlows)
            {
                ImGui::PushID(currentIterator.first.data());
                ImGui::Selectable(currentIterator.first.data());

                static bool sTriggerRenamePopup = false;

                if(ImGui::BeginPopupContextItem())
                {
                    if(ImGui::MenuItem("Delete"))
                    {
                        this->RemoveFlow(currentIterator.first);
                    }

                    if(ImGui::MenuItem("Rename"))
                    {
                        sTriggerRenamePopup |= true;
                    }

                    ImGui::EndPopup();
                }

                if(sTriggerRenamePopup)
                {
                    ImGui::OpenPopup("RenamePopup");
                }

                if(ImGui::BeginPopupModal("RenamePopup"))
                {
                    static char sNameBuffer[512] = {0};

                    if(sTriggerRenamePopup == true)
                    {
                        sTriggerRenamePopup = false;
                        strcpy(sNameBuffer, currentIterator.first.data());
                    }

                    ImGui::InputText("Name", sNameBuffer, sizeof(sNameBuffer) - 1);
                    ImGui::SameLine();
                    if(ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Enter))
                    {
                        GenesisFlow* renameTarget = m_Flows.at(currentIterator.first);
                        m_Flows.erase(currentIterator.first);
                        m_Flows.emplace(sNameBuffer, renameTarget);
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Content"))
        {
        }
        ImGui::End();
    }

    GenesisFlow* GenesisBundleEditor::sfDefaultFactory(void* Reserved)
    {
        return new GenesisFlowEditor();
    }

} // namespace genesis::editor