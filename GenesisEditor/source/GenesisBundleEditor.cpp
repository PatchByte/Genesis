#include "GenesisEditor/GenesisBundleEditor.hpp"
#include "Ash/AshResult.h"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstring>

namespace genesis::editor
{

    GenesisBundleEditor::GenesisBundleEditor(utils::GenesisLogBox* LogBox)
        : GenesisBundle(GenesisBundleEditor::sfDefaultFactory), m_DockSpaceHasBeenBuilt(false), m_DockSpaceId(0), m_DockSidebarWindow(0), m_DockContentWindow(0), m_DockLogWindow(0), m_SelectedFlow(),
          m_LogBox(LogBox), m_Logger("GenesisBundleEditor", {}), m_SearchBoxName()
    {
        m_ReservedFactoryValue = this;

        m_Logger.AddLoggerPassage(m_LogBox->CreatePassage());
    }

    GenesisBundleEditor::~GenesisBundleEditor()
    {
        this->Shutdown();
    }

    ash::AshResult GenesisBundleEditor::CreateFlow(std::string FlowName)
    {
        return GenesisBundle::CreateFlow(FlowName);
    }

    bool GenesisBundleEditor::Import(ash::AshStream* Stream)
    {
        return GenesisBundle::Import(Stream);
    }

    void GenesisBundleEditor::Initialize()
    {
        // Initialization

        for (auto currentIterator : m_Flows)
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

            // Sidebar upper separator

            ImGui::Text("Flows");

            ImGui::Separator();

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

                    if (ImGui::MenuItem("Extract"))
                    {
                        IGFD::FileDialogConfig config = IGFD::FileDialogConfig();
                        config.path = ".";

                        config.userDatas = new std::string(currentIterator.first);

                        ImGuiFileDialog::Instance()->OpenDialog("ExtractFlowDialogKey", "Extract Flow", ".genesis_flow", config);
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
                        if (m_Flows.contains(sNameBuffer) == false)
                        {
                            GenesisFlowEditor* renameTarget = dynamic_cast<GenesisFlowEditor*>(m_Flows.at(currentIterator.first));

                            m_Flows.erase(currentIterator.first);
                            m_Flows.emplace(sNameBuffer, renameTarget);

                            ImGui::CloseCurrentPopup();

                        }
                        else
                        {
                            m_Logger.Log("Error", "Flow with the name already exists.");
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
                    {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }

            if (ImGuiFileDialog::Instance()->Display("ExtractFlowDialogKey", ImGuiWindowFlags_NoCollapse, {700, 350}))
            {
                std::string* flowName = reinterpret_cast<std::string*>(ImGuiFileDialog::Instance()->GetUserDatas());

                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    this->ExtractFlowAndSaveToFileAndApplyLogs(*flowName, ImGuiFileDialog::Instance()->GetFilePathName());
                }

                delete flowName;

                ImGuiFileDialog::Instance()->Close();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Content"))
        {
            if (auto unCastSelectedFlow = GetFlow(m_SelectedFlow))
            {
                GenesisFlowEditor* selectedFlow = dynamic_cast<GenesisFlowEditor*>(unCastSelectedFlow);

                selectedFlow->Render(m_SelectedFlow);
            }
        }
        ImGui::End();

        if (ImGui::Begin("Log"))
        {
            if (m_LogBox)
            {
                m_LogBox->Render();
            }
        }
        ImGui::End();
    }

    ash::AshResult GenesisBundleEditor::ExtractFlowAndSaveToFile(std::string FlowName, std::filesystem::path Output)
    {
        if (auto flow = this->GetFlow(FlowName))
        {
            ash::AshStreamExpandableExportBuffer exportBufferStream = ash::AshStreamExpandableExportBuffer();

            if (flow->Export(&exportBufferStream))
            {
                if (auto exportBuffer = exportBufferStream.MakeCopyOfBuffer())
                {
                    exportBuffer->WriteToFile(Output);

                    delete exportBuffer;

                    return ash::AshResult(false, "Successfully exported.");
                }
            }
            else
            {
                return ash::AshResult(false, "Failed to export flow.");
            }
        }
        else
        {
            return ash::AshResult(false, "Failed to find flow.");
        }

        // This should not be reached.
        return ash::AshResult(false);
    }

    ash::AshResult GenesisBundleEditor::ExtractFlowAndSaveToFileAndApplyLogs(std::string FlowName, std::filesystem::path Output)
    {
        if (auto res = this->ExtractFlowAndSaveToFile(FlowName, Output); res.WasSuccessful())
        {
            m_Logger.Log("Info", res.GetMessage());
            return std::move(res);
        }
        else
        {
            m_Logger.Log("Error", res.GetMessage());
            return std::move(res);
        }
    }

    GenesisFlow* GenesisBundleEditor::sfDefaultFactory(void* Reserved)
    {
        GenesisBundleEditor* bundleEditor = reinterpret_cast<GenesisBundleEditor*>(Reserved);
        GenesisFlowEditor* flowEditor = new GenesisFlowEditor(bundleEditor->m_LogBox);

        flowEditor->Initialize();

        return flowEditor;
    }

} // namespace genesis::editor
