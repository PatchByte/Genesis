#include "GenesisEditor/GenesisEditor.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshStream.h"
#include "GenesisRenderer/GenesisRenderer.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <filesystem>
#include <thread>

namespace ed = ax::NodeEditor;

namespace genesis::editor
{

    GenesisEditor::GenesisEditor() : m_LogBox(), m_Logger("GenesisEditor", {}), m_TestBundleEditor(&m_LogBox)
    {
        m_Renderer = renderer::GenesisRendererProvider::CreateRenderer(1600, 900, "Genesis Editor");
        m_Logger.AddLoggerPassage(m_LogBox.CreatePassage());
    }

    GenesisEditor::~GenesisEditor()
    {

        if (m_Renderer)
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

            if (ImGui::Begin("GenesisEditorFrame", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar))
            {
                ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
                ImGui::SetWindowPos(ImVec2(0, 0));

                static bool sTriggerNewPopup = false;

                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("File"))
                    {
                        if (ImGui::MenuItem("Open"))
                        {
                            ImGuiFileDialog::Instance()->OpenDialog("OpenBundleDialogKey", "Open File", ".genesis");
                        }

                        if (ImGui::MenuItem("Save"))
                        {
                            ImGuiFileDialog::Instance()->OpenDialog("SaveBundleDialogKey", "Save File", ".genesis");
                        }

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Flows"))
                    {
                        if (ImGui::MenuItem("New"))
                        {
                            sTriggerNewPopup = true;
                        }

                        ImGui::EndMenu();
                    }

                    ImGui::EndMenuBar();
                }

                if (sTriggerNewPopup)
                {
                    ImGui::OpenPopup("NewPopup");
                    sTriggerNewPopup = false;
                }

                if (ImGui::BeginPopupModal("NewPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    static char sNameBuffer[512] = {0};

                    ImGui::InputText("Name", sNameBuffer, sizeof(sNameBuffer) - 1);
                    ImGui::SameLine();
                    if (ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Enter))
                    {
                        m_TestBundleEditor.CreateFlow(sNameBuffer);
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
                    {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                m_TestBundleEditor.Render();

                ImGui::End();
            }

            if (ImGuiFileDialog::Instance()->Display("OpenBundleDialogKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    ash::AshStreamExpandableExportBuffer expandableStream = ash::AshStreamExpandableExportBuffer();
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

                    if (std::filesystem::exists(filePathName))
                    {
                        ash::AshBuffer fileBuffer = ash::AshBuffer();

                        if (fileBuffer.ReadFromFile(filePathName).WasSuccessful())
                        {
                            ash::AshStreamStaticBuffer fileBufferStream = ash::AshStreamStaticBuffer(&fileBuffer, ash::AshStreamMode::READ);

                            if (m_TestBundleEditor.Import(&fileBufferStream))
                            {
                                m_Logger.Log("Info", "Loaded file {}.", filePathName);
                            }
                            else
                            {
                                m_Logger.Log("Error", "Failed to deserialize file {}.", filePathName);
                            }
                        }
                        else
                        {
                            m_Logger.Log("Error", "Failed to read file {}.", filePathName);
                        }
                    }
                    else
                    {
                        m_Logger.Log("Error", "File {} does not exist.", filePathName);
                    }
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGuiFileDialog::Instance()->Display("SaveBundleDialogKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    ash::AshStreamExpandableExportBuffer expandableStream = ash::AshStreamExpandableExportBuffer();
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

                    if (m_TestBundleEditor.Export(&expandableStream))
                    {
                        if (auto exportBuffer = expandableStream.MakeCopyOfBuffer())
                        {
                            if (exportBuffer->WriteToFile(filePathName).WasSuccessful())
                            {
                                m_Logger.Log("Info", "Saved to {}.", filePathName);
                            }
                            else
                            {
                                m_Logger.Log("Error", "Failed to save to {}.", filePathName);
                            }

                            delete exportBuffer;
                        }
                    }
                    else
                    {
                        m_Logger.Log("Error", "Failed to export bundle.");
                    }
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            m_Renderer->EndFrame();

            std::this_thread::yield();
        }

        m_TestBundleEditor.Shutdown();

        m_Renderer->Shutdown();
    }

} // namespace genesis::editor