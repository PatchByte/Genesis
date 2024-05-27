#include "GenesisEditor/GenesisEditor.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisUtils.hpp"
#include "GenesisEditor/GenesisWidgets.hpp"
#include "GenesisEditor/live/GenesisLive.hpp"
#include "GenesisOutput/GenesisOutputBuilder.hpp"
#include "GenesisRenderer/GenesisRenderer.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "ImGuiFileDialog.h"
#include "fmt/format.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include <filesystem>
#include <fstream>
#include <thread>

namespace ed = ax::NodeEditor;

namespace genesis::editor
{

    GenesisEditor::GenesisEditor() : m_LogBox(), m_Logger("GenesisEditor", {}), m_TestBundleEditor(&m_LogBox), m_ForceDisableRendering(false), m_Live(nullptr)
    {
        m_Renderer = renderer::GenesisRendererProvider::CreateRenderer(1600, 900, "Genesis Editor");
        m_Logger.AddLoggerPassage(m_LogBox.CreatePassage());
    }

    GenesisEditor::~GenesisEditor()
    {
        if (m_Live)
        {
            delete m_Live;
            m_Live = nullptr;
        }

        if (m_Renderer)
        {
            delete m_Renderer;
            m_Renderer = nullptr;
        }
    }

    void GenesisEditor::Run(int ArgCount, const char** ArgArray)
    {
        // Live

        m_Live = new live::GenesisLive(&m_LogBox, utils::GenesisUtils::sfGetUsername());

        // Renderer

        m_Renderer->Initialize();

        m_Renderer->SetDropFileHandler(
            [this](int FilePathCount, const char** FilePathArray) -> void
            {
                for (int currentFilePathIndex = 0; currentFilePathIndex < FilePathCount; currentFilePathIndex++)
                {
                    if (std::string currentFilePath = FilePathArray[currentFilePathIndex]; std::filesystem::exists(currentFilePath))
                    {
                        if (currentFilePath.ends_with(".genesis"))
                        {
                            this->LoadGenesisFileFromAndApplyLogs(FilePathArray[currentFilePathIndex]);
                        }
                        else if (currentFilePath.ends_with(".exe") || currentFilePath.ends_with(".dll"))
                        {
                            this->ProcessGenesisFileAndApplyLogs(FilePathArray[currentFilePathIndex]);
                        }
                        else
                        {
                            m_Logger.Log("Error", "Unknown file {}.", FilePathArray[currentFilePathIndex]);
                        }
                    }
                }
            });

        // Fonts

        m_KeyboardFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/212Keyboard-lmRZ.otf", 16.f);
        m_DefaultFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/Cantarell-Regular.ttf", 22.);

        ImGui::GetIO().FontDefault = m_DefaultFont;

        // Initialization

        m_TestBundleEditor.Initialize(m_KeyboardFont);

        if (ArgCount > 1)
        {
            LoadGenesisFileFromAndApplyLogs(ArgArray[1]);
        }

        // Stylization

        this->ApplyStyle();

        while (m_Renderer->ShallRender())
        {
            m_Renderer->BeginFrame();

            if (m_ForceDisableRendering == false)
            {
                if (ImGui::Begin("GenesisEditorFrame", nullptr,
                                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar))
                {
                    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
                    ImGui::SetWindowPos(ImVec2(0, 0));

                    static bool sTriggerFlowNewNodePopup = false;
                    static bool sTriggerLiveConnectPopup = false;
                    static bool sTriggerLiveHintConnectPopup = false;

                    if (ImGui::BeginMenuBar())
                    {
                        if (ImGui::BeginMenu("File"))
                        {
                            if (ImGui::MenuItem("Open"))
                            {
                                ImGuiFileDialog::Instance()->OpenDialog("OpenBundleDialogKey", "Open File", ".genesis");
                            }

                            if (ImGui::MenuItem("Save as"))
                            {
                                ImGuiFileDialog::Instance()->OpenDialog("SaveBundleDialogKey", "Save File", ".genesis");
                            }

                            if (ImGui::MenuItem("Save", nullptr, false, m_LastSavedAsFile.empty() == false))
                            {
                                this->SaveGenesisFileToAndApplyLogs(m_LastSavedAsFile);
                            }

                            if (ImGui::MenuItem("Process specific file"))
                            {
                                ImGuiFileDialog::Instance()->OpenDialog("ProcessBundleDialogKey", "Process File", ".exe,.dll");
                            }

                            if (ImGui::MenuItem("Process last file", nullptr, false, m_LastProcessedFile.empty() == false))
                            {
                                if (auto res = this->ProcessGenesisFileAndApplyLogs(m_LastProcessedFile); res.WasSuccessful())
                                {
                                    IGFD::FileDialogConfig config = IGFD::FileDialogConfig();
                                    config.userDatas = new std::string(res.GetResult());

                                    ImGuiFileDialog::Instance()->OpenDialog("SaveOutputDialogKey", "Save Output Header File", ".hpp,.h", config);
                                }
                            }

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Flow"))
                        {
                            bool hasFlowSelected = m_TestBundleEditor.HasFlowSelected();

                            if (ImGui::MenuItem("New"))
                            {
                                sTriggerFlowNewNodePopup = true;
                            }

                            if (ImGui::MenuItem("Navigate to Content", "PageDown", false, hasFlowSelected))
                            {
                                m_TestBundleEditor.GetSelectedFlow()->DoAction(GenesisFlowEditor::ActionType::NAVIGATE_TO_CONTENT, nullptr);
                            }

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Log"))
                        {
                            if (ImGui::MenuItem("Clear"))
                            {
                                m_LogBox.Clear();
                            }

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Live"))
                        {
                            bool shouldEnableRequiredWaitingOptions = false;
                            bool shouldEnableRequiredAuthedOptions = false;

                            if (m_Live->GetRelayConnection() != nullptr)
                            {
                                shouldEnableRequiredWaitingOptions = m_Live->GetRelayConnection()->IsWaiting();
                                shouldEnableRequiredAuthedOptions = m_Live->GetRelayConnection()->IsAuthed();
                            }

                            if (ImGui::MenuItem("Connect"))
                            {
                                sTriggerLiveConnectPopup = true;
                            }

                            if (ImGui::MenuItem("Copy Invite Code", nullptr, false, shouldEnableRequiredWaitingOptions))
                            {
                                ImGui::SetClipboardText(m_Live->GetRelayConnection()->GetMyConnectionString().data());
                            }

                            if(ImGui::MenuItem("Invite Other Client", nullptr, false, shouldEnableRequiredAuthedOptions))
                            {
                                sTriggerLiveHintConnectPopup = true;
                            }

                            ImGui::EndMenu();
                        }

                        ImGui::EndMenuBar();
                    }

                    // Popup triggers

                    if (sTriggerFlowNewNodePopup)
                    {
                        ImGui::OpenPopup("FlowNewNodePopup");
                        sTriggerFlowNewNodePopup = false;
                    }

                    if (sTriggerLiveConnectPopup)
                    {
                        ImGui::OpenPopup("LiveConnectPopup");
                        sTriggerLiveConnectPopup = false;
                    }

                    if(sTriggerLiveHintConnectPopup)
                    {
                        ImGui::OpenPopup("Invite Other Client");
                        sTriggerLiveHintConnectPopup = false;
                    }

                    if (ImGui::BeginPopup("FlowNewNodePopup", ImGuiWindowFlags_AlwaysAutoResize))
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

                    if (ImGui::BeginPopup("LiveConnectPopup"))
                    {
                        static std::string inviteCodeInput = "";

                        widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Invite Code", &inviteCodeInput);

                        if (ImGui::Button("Connect") || ImGui::IsKeyPressed(ImGuiKey_Enter))
                        {
                            if (auto res = m_Live->InitializeConnection(inviteCodeInput); res.HasError())
                            {
                                m_Logger.Log("Error", "Failed to initialize connection. {}", res.GetMessage());
                            }

                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
                        {
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::EndPopup();
                    }
                    
                    if(ImGui::BeginPopupModal("Invite Other Client"))
                    {
                        static std::string inviteCodeInput = "";

                        ImGui::Text("Please send him the following invite code.");
                        
                        if(ImGui::Button("Copy Invite Code"))
                        {
                            ImGui::SetClipboardText(m_Live->GetRelayConnectionInviteCode().data());
                        }

                        ImGui::Separator();

                        ImGui::Text("Once done, please paste in his invite code and click invite.");

                        widgets::GenesisGenericWidgets::sfRenderInputTextStlString("Invite Code", &inviteCodeInput);

                        if(ImGui::Button("Invite"))
                        {
                            if(auto res = m_Live->InviteHintConnection(inviteCodeInput); res.HasError())
                            {
                                m_Logger.Log("Error", "Failed to invite (hint) client. {}", res.GetMessage());
                            }
                            else
                            {
                                m_Logger.Log("Info", "Invited (hinted) client.");
                            }

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
                }
                ImGui::End();
            }

            if (ImGuiFileDialog::Instance()->Display("OpenBundleDialogKey", ImGuiWindowFlags_NoCollapse, {700, 350}))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    this->LoadGenesisFileFromAndApplyLogs(ImGuiFileDialog::Instance()->GetFilePathName());
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGuiFileDialog::Instance()->Display("SaveBundleDialogKey", ImGuiWindowFlags_NoCollapse, {700, 350}))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    this->SaveGenesisFileToAndApplyLogs(ImGuiFileDialog::Instance()->GetFilePathName());
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGuiFileDialog::Instance()->Display("ProcessBundleDialogKey", ImGuiWindowFlags_NoCollapse, {700, 350}))
            {
                bool openSaveOutputDialog = false;
                void* openSaveOutputDialogUserData = nullptr;

                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    if (auto res = this->ProcessGenesisFileAndApplyLogs(ImGuiFileDialog::Instance()->GetFilePathName()); res.WasSuccessful())
                    {
                        openSaveOutputDialog = true;
                        openSaveOutputDialogUserData = new std::string(res.GetResult());
                    }
                }

                // close
                ImGuiFileDialog::Instance()->Close();

                if (openSaveOutputDialog)
                {
                    IGFD::FileDialogConfig config = IGFD::FileDialogConfig();
                    config.userDatas = openSaveOutputDialogUserData;

                    ImGuiFileDialog::Instance()->OpenDialog("SaveOutputDialogKey", "Save Output Header File", ".hpp,.h", config);
                }
            }

            if (ImGuiFileDialog::Instance()->Display("SaveOutputDialogKey", ImGuiWindowFlags_NoCollapse, {700, 350}))
            {
                std::string* outputCode = reinterpret_cast<std::string*>(ImGuiFileDialog::Instance()->GetUserDatas());

                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    std::ofstream outputStream = std::ofstream(ImGuiFileDialog::Instance()->GetFilePathName(), std::ios::trunc);

                    outputStream << *outputCode << std::endl;

                    outputStream.flush();
                    outputStream.close();
                }

                delete outputCode;

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            m_Renderer->EndFrame();

            std::this_thread::yield();
        }

        m_TestBundleEditor.Shutdown();

        m_Renderer->Shutdown();
    }

    ash::AshResult GenesisEditor::LoadGenesisFileFrom(std::filesystem::path Path)
    {
        ash::AshStreamExpandableExportBuffer expandableStream = ash::AshStreamExpandableExportBuffer();
        std::string filePathName = Path.string();

        if (std::filesystem::exists(filePathName))
        {
            m_ForceDisableRendering = true;

            ash::AshBuffer fileBuffer = ash::AshBuffer();

            if (fileBuffer.ReadFromFile(filePathName).WasSuccessful())
            {
                ash::AshStreamStaticBuffer fileBufferStream = ash::AshStreamStaticBuffer(&fileBuffer, ash::AshStreamMode::READ);

                if (m_TestBundleEditor.Import(&fileBufferStream))
                {
                    m_ForceDisableRendering = false;
                    return ash::AshResult(true, fmt::format("Loaded file {}.", filePathName));
                }
                else
                {
                    m_ForceDisableRendering = false;
                    return ash::AshResult(false, fmt::format("Failed to deserialize file {}.", filePathName));
                }
            }
            else
            {
                m_ForceDisableRendering = false;
                return ash::AshResult(false, fmt::format("Failed to read file {}.", filePathName));
            }
        }
        else
        {
            return ash::AshResult(false, fmt::format("File {} does not exist.", filePathName));
        }

        // This should not be reached.
        return ash::AshResult(false);
    }

    ash::AshResult GenesisEditor::SaveGenesisFileTo(std::filesystem::path Path)
    {
        ash::AshStreamExpandableExportBuffer expandableStream = ash::AshStreamExpandableExportBuffer();
        std::string filePathName = Path.string();

        if (m_TestBundleEditor.Export(&expandableStream))
        {
            if (auto exportBuffer = expandableStream.MakeCopyOfBuffer())
            {
                if (exportBuffer->WriteToFile(filePathName).WasSuccessful())
                {
                    m_LastSavedAsFile = filePathName;
                    return ash::AshResult(true, fmt::format("Saved to {}.", filePathName));
                }
                else
                {
                    return ash::AshResult(false, fmt::format("Failed to save to {}.", filePathName));
                }

                delete exportBuffer;
            }
        }
        else
        {
            return ash::AshResult(false, "Failed to export bundle.");
        }

        // This should be reached.
        return ash::AshResult(false);
    }

    ash::AshCustomResult<std::string> GenesisEditor::ProcessGenesisFile(std::filesystem::path Input)
    {
        if (std::filesystem::exists(Input))
        {
            common::GenesisLoadedFile* loadedFile = new common::GenesisLoadedFile(Input);

            if (auto res = loadedFile->Load(); res.HasError())
            {
                return ash::AshCustomResult<std::string>(false, fmt::format("Failed to load file {}. {}", Input.string(), res.GetMessage()));
            }

            output::GenesisOutputData* outputData = new output::GenesisOutputData();

            if (auto res = m_TestBundleEditor.ProcessBundle(outputData, loadedFile); res.HasError())
            {
                return ash::AshCustomResult<std::string>(false, fmt::format("Failed processing bundle. {}", res.GetMessage()));
            }

            std::string outputCode = output::GenesisOutputBuilder::Build(outputData);

            delete outputData;
            delete loadedFile;

            m_LastProcessedFile = Input;

            return ash::AshCustomResult<std::string>(true, "Successfully processed.").AttachResult(outputCode);
        }
        else
        {
            return ash::AshCustomResult<std::string>(false, "Input file does not exist.");
        }

        // This should not be reached.
        return ash::AshCustomResult<std::string>(false);
    }

    ash::AshResult GenesisEditor::LoadGenesisFileFromAndApplyLogs(std::filesystem::path Path)
    {
        if (auto res = this->LoadGenesisFileFrom(Path); res.WasSuccessful())
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

    ash::AshResult GenesisEditor::SaveGenesisFileToAndApplyLogs(std::filesystem::path Path)
    {
        if (auto res = this->SaveGenesisFileTo(Path); res.WasSuccessful())
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

    ash::AshCustomResult<std::string> GenesisEditor::ProcessGenesisFileAndApplyLogs(std::filesystem::path Input)
    {
        if (auto res = this->ProcessGenesisFile(Input); res.WasSuccessful())
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

} // namespace genesis::editor