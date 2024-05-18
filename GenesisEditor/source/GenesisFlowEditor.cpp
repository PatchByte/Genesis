#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshStream.h"
#include "AshObjects/AshString.h"
#include "GenesisEditor/GenesisNodeBuilder.hpp"
#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisPinTracker.hpp"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_canvas.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

namespace ed = ax::NodeEditor;

namespace genesis::editor
{

    GenesisFlowEditor::GenesisFlowEditor(utils::GenesisLogBox* LogBox)
        : GenesisFlow(), m_Logger("GuiLogger", {}), m_LogBox(LogBox), m_TriggerCheck(false), m_TriggerActionFocusFirstNode(false), m_Canvas()
    {
        AddOperationToFlow(new operations::GenesisFindPatternOperation("E8 ? ? ? ? 90"));
        AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 6));
        AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 2));

        if (m_LogBox)
        {
            m_Logger.AddLoggerPassage(m_LogBox->CreatePassage());
        }
    }

    GenesisFlowEditor::~GenesisFlowEditor()
    {
    }

    void GenesisFlowEditor::Initialize()
    {
        ed::Config nodeEditorConfig = ed::Config();

        nodeEditorConfig.EnableSmoothZoom = false;
        nodeEditorConfig.UserPointer = this;

        nodeEditorConfig.LoadNodeSettings = [](ed::NodeId NodeId, char* Data, void* UserPointerUnCast) -> size_t
        {
            GenesisFlowEditor* userPointer = static_cast<GenesisFlowEditor*>(UserPointerUnCast);

            if (userPointer->m_NodeEditorSavedStates.contains(NodeId.Get()) == false)
            {
                return 0;
            }

            if (Data)
            {
                memcpy(Data, userPointer->m_NodeEditorSavedStates.at(NodeId.Get()).data(), userPointer->m_NodeEditorSavedStates.at(NodeId.Get()).size());
            }

            return userPointer->m_NodeEditorSavedStates.at(NodeId.Get()).size();
        };

        nodeEditorConfig.SaveNodeSettings = [](ed::NodeId NodeId, const char* Data, size_t Size, ed::SaveReasonFlags Reason, void* UserPointerUnCast) -> bool
        {
            GenesisFlowEditor* userPointer = static_cast<GenesisFlowEditor*>(UserPointerUnCast);

            std::string state = "";
            state.assign(Data, Size);

            userPointer->m_NodeEditorSavedStates.emplace(NodeId.Get(), state);

            return true;
        };

        m_NodeEditorContext = ed::CreateEditor(&nodeEditorConfig);
    }

    void GenesisFlowEditor::Shutdown()
    {
        ed::DestroyEditor(m_NodeEditorContext);
    }

    void GenesisFlowEditor::Render()
    {

        RenderNodes();
    }

    void GenesisFlowEditor::RenderNodes()
    {
        if (m_TriggerCheck)
        {
            m_LogBox->Clear();
            m_TriggerCheck = false;

            if (auto resIsRunnable = this->CheckIfFlowIsRunnable(); resIsRunnable.HasError())
            {
                m_Logger.Log("Error", "Flow is not runnable. {}", resIsRunnable.GetMessage());
            }
            else
            {
                m_Logger.Log("Info", "Flow is okay!");
            }
        }

        // if (m_Canvas.Begin("##EditorCanvas", {-1, -1}))
        {

            ed::SetCurrentEditor(m_NodeEditorContext);
            ed::GetStyle().NodeRounding = 5.f;

            ed::Begin("MainEditor", {-1, -1});

            utils::GenesisNodeBuilder nodeBuilder = utils::GenesisNodeBuilder();

            for (auto currentIterator : m_Operations)
            {
                ImColor normalColor = ImColor();

                sfGetColorForOperationInformation(currentIterator.second->GetOperationInformation(), &normalColor);

                ImGui::PushID(currentIterator.first);

                nodeBuilder.Begin(currentIterator.first);

                nodeBuilder.Header("", normalColor);
                ImGui::BeginGroup();
                ImGui::Text("%s (%i)", currentIterator.second->GetHumanReadableName().data(), currentIterator.first);
                ImGui::Dummy({0.f, 2.f});
                ImGui::EndGroup();
                nodeBuilder.EndHeader();

                RenderNodeOperation(nodeBuilder, currentIterator.second);
                // RenderNodeOperation takes care of that now
                // nodeBuilder.End();

                ImGui::PopID();
            }

            for (auto currentIterator : m_Links)
            {
                const std::pair<uintptr_t, uintptr_t> p = currentIterator.second;
                ed::Link(currentIterator.first, p.first, p.second);
            }

            // Post-Actions

            if (ed::BeginCreate())
            {
                ed::PinId startPinId, endPinId;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    utils::GenesisPinValue startPinParsed = startPinId.Get();
                    utils::GenesisPinValue endPinParsed = endPinId.Get();

                    if (startPinParsed.m_NodePinType == utils::GenesisPinType::INPUT && endPinParsed.m_NodePinType == utils::GenesisPinType::OUTPUT)
                    {
                        ed::PinId carry = startPinId;
                        startPinId = endPinId;
                        endPinId = carry;

                        startPinParsed = startPinId.Get();
                        endPinParsed = endPinId.Get();
                    }

                    if (startPinId && endPinId && startPinParsed.m_NodePinType == utils::GenesisPinType::OUTPUT && endPinParsed.m_NodePinType == utils::GenesisPinType::INPUT)
                    {
                        if (ed::AcceptNewItem())
                        {
                            ax::NodeEditor::LinkId linkId = ++m_CounterLinks;

                            m_Links.emplace(linkId, std::make_pair(startPinId.Get(), endPinId.Get()));
                            m_TriggerCheck = true;
                        }
                    }
                }
            }
            ed::EndCreate();

            if (ed::BeginDelete())
            {
                ed::LinkId deletedLinkId;

                if (ed::QueryDeletedLink(&deletedLinkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        if (m_Links.contains(deletedLinkId.Get()))
                        {
                            m_Links.erase(deletedLinkId.Get());
                        }
                        else
                        {
                            printf("Destroying failed: %li\n", deletedLinkId.Get());
                        }

                        m_TriggerCheck = true;
                    }
                }
            }
            ed::EndDelete();

            {

                bool actionDelete = ImGui::IsKeyPressed(ImGuiKey_Delete);

                if (auto numSelectedLinks = ed::GetSelectedLinks(nullptr, 0); numSelectedLinks > 0)
                {
                    ed::LinkId* selectedLinks = new ed::LinkId[numSelectedLinks];
                    ed::GetSelectedLinks(selectedLinks, numSelectedLinks);

                    for (int currentSelectedLinkIndex = 0; currentSelectedLinkIndex < numSelectedLinks; currentSelectedLinkIndex++)
                    {
                        ed::LinkId selectedLink = selectedLinks[currentSelectedLinkIndex];

                        if (actionDelete)
                        {
                            // m_Links.erase(selectedLink.Get());
                        }
                    }
                }

                if (auto numSelectedNodes = ed::GetSelectedNodes(nullptr, 0); numSelectedNodes > 0)
                {
                    ed::NodeId* selectedNodes = new ed::NodeId[numSelectedNodes];
                    ed::GetSelectedNodes(selectedNodes, numSelectedNodes);

                    for (int currentSelectedNodeIndex = 0; currentSelectedNodeIndex < numSelectedNodes; currentSelectedNodeIndex++)
                    {
                        ed::NodeId selectedNode = selectedNodes[currentSelectedNodeIndex];

                        if (actionDelete)
                        {
                            RemoveOperationFromFlow(selectedNode.Get());
                        }
                    }
                }
            }

            // Debug

            if (m_TriggerActionFocusFirstNode || ImGui::IsKeyPressed(ImGuiKey_PageDown))
            {
                if (m_Operations.size() > 0)
                {
                    ed::CenterNodeOnScreen(m_Operations.begin()->second->GetOperationId());
                }
            }

            // New Action

            ed::Suspend();

            if (ed::ShowBackgroundContextMenu())
            {
                ImGui::OpenPopup("New ##PopupNodeEditorNewNode");
            }

            if (ImGui::BeginPopup("New ##PopupNodeEditorNewNode"))
            {
                static std::map<std::string, operations::GenesisOperationType> sNewItems = {
                    { "Pattern", operations::GenesisOperationType::FIND_PATTERN },
                    { "Math", operations::GenesisOperationType::MATH }
                };

                for(auto currentIterator : sNewItems)
                {
                    if(ImGui::MenuItem(currentIterator.first.data()))
                    {
                        operations::GenesisOperationId newOperationId = AddOperationToFlow(operations::GenesisOperationUtils::sfCreateOperationByType(currentIterator.second));
                        ed::SetNodePosition(newOperationId, ed::ScreenToCanvas(ImGui::GetMousePos()));
                    }
                }

                ImGui::EndPopup();
            }

            ed::Resume();

            ed::End();
            ed::SetCurrentEditor(nullptr);
            // m_Canvas.End();

            
        }
    }

    void GenesisFlowEditor::RenderNodeOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation)
    {
        GenesisOperationEditorForNodes::sfRenderOperation(Builder, Operation);
    }

    bool GenesisFlowEditor::sfGetColorForOperationInformation(const operations::GenesisOperationInformation& Information, ImColor* OutputNormalColor)
    {
        float nH = 183.f;

        if (Information.m_IsFlowStartNode)
        {
            nH += 100.f;
        }

        if (Information.m_IsConditionalFlowStartNode)
        {
            nH += 200.f;
        }

        if (Information.m_IsMathOperation)
        {
            nH += 50.f;
        }

        nH = fmodf(nH, 360.f);

        float h = nH / 360.f;
        float s = 100.f / 100.f;
        float v = 65.f / 100.f;

        ImGui::ColorConvertHSVtoRGB(h, s, v, OutputNormalColor->Value.x, OutputNormalColor->Value.y, OutputNormalColor->Value.z);

        OutputNormalColor->Value.w = 1.f;

        return true;
    }

    // Obligatory Save/Load stuff

    void GenesisFlowEditor::Reset()
    {
        m_NodeEditorSavedStates.clear();

        GenesisFlow::Reset();
    }

    bool GenesisFlowEditor::Import(ash::AshStream* Stream)
    {
        bool res = GenesisFlow::Import(Stream);

        if (res)
        {
            ash::AshStreamStaticBuffer reservedBufferGuiStream = ash::AshStreamStaticBuffer(&m_ReservedBufferGui, ash::AshStreamMode::READ);

            size_t nodeStatesSize = reservedBufferGuiStream.Read<size_t>();

            for (size_t currentNodeStateIndex = 0; currentNodeStateIndex < nodeStatesSize; currentNodeStateIndex++)
            {
                uintptr_t currentNodeStateId = reservedBufferGuiStream.Read<uintptr_t>();
                ash::objects::AshAsciiString currentNodeDataString = ash::objects::AshAsciiString();

                if (currentNodeDataString.Import(&reservedBufferGuiStream) == false)
                {
                    std::cout << "Failed to import currentNodeDataString" << std::endl;
                    return false;
                }

                m_NodeEditorSavedStates.emplace(currentNodeStateId, currentNodeDataString.GetText());
            }
        }

        for (auto currentIterator : m_NodeEditorSavedStates)
        {
            auto currentContext = ed::GetCurrentEditor();
            ed::SetCurrentEditor(m_NodeEditorContext);
            ed::RestoreNodeState(currentIterator.first);
            ed::SetCurrentEditor(currentContext);
        }

        return res;
    }

    bool GenesisFlowEditor::Export(ash::AshStream* Stream)
    {
        {
            ash::AshStreamExpandableExportBuffer reservedBufferGuiStream = ash::AshStreamExpandableExportBuffer();

            reservedBufferGuiStream.Write<size_t>(m_NodeEditorSavedStates.size());

            for (auto currentIterator : m_NodeEditorSavedStates)
            {
                reservedBufferGuiStream.Write(currentIterator.first);
                ash::objects::AshAsciiString(currentIterator.second).Export(&reservedBufferGuiStream);
            }

            if (auto res = reservedBufferGuiStream.MakeCopyOfBuffer(); res)
            {
                m_ReservedBufferGui.CopyAshBufferFromPointer(res);
                delete res;
            }
        }

        return GenesisFlow::Export(Stream);
    }

} // namespace genesis::editor