#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "AshObjects/AshString.h"
#include "GenesisEditor/GenesisNodeBuilder.hpp"
#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisEditor/live/GenesisLiveConnection.hpp"
#include "GenesisEditor/live/GenesisLivePackets.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisPinTracker.hpp"
#include "imgui.h"
#include "imgui_node_editor.h"
#include <cmath>
#include <cstddef>
#include <map>
#include <utility>

namespace ed = ax::NodeEditor;

namespace genesis::editor
{

    GenesisFlowEditor::GenesisFlowEditor(utils::GenesisLogBox* LogBox)
        : GenesisFlow(), m_Logger("GuiLogger", {}), m_LogBox(LogBox), m_NodeEditorContext(nullptr), m_TriggerCheck(false), m_TriggerActionFocusFirstNode(false), m_TriggerRestoreStateOfNodes(false),
          m_Live(nullptr), m_LiveFlowName()
    {
        CreateOperationInFlowFromType(operations::GenesisOperationType::FIND_PATTERN);
        // AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 6));
        // AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 2));

        if (m_LogBox)
        {
            m_Logger.AddLoggerPassage(m_LogBox->CreatePassage());
        }
    }

    GenesisFlowEditor::~GenesisFlowEditor()
    {
        this->Shutdown();
    }

    void GenesisFlowEditor::Initialize()
    {
        ed::Config nodeEditorConfig = ed::Config();

        nodeEditorConfig.EnableSmoothZoom = false;
        nodeEditorConfig.UserPointer = this;
        nodeEditorConfig.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
        {
            GenesisFlowEditor* flowEditor = reinterpret_cast<GenesisFlowEditor*>(userPointer);

            if (static_cast<unsigned int>(reason) & static_cast<unsigned int>(ed::SaveReasonFlags::Position))
            {
                ImVec2 position = ed::GetNodePosition(nodeId);

                live::GenesisLiveConnectionPacketFlowAction packetFlowAction = live::GenesisLiveConnectionPacketFlowAction();
                packetFlowAction.SetAsActionSetPositionNode({.m_NodeId = nodeId.Get(), .m_X = position.x, .m_Y = position.y});
                flowEditor->SendLiveFlowAction(packetFlowAction);
            }

            return false;
        };

        m_NodeEditorContext = ed::CreateEditor(&nodeEditorConfig);
    }

    void GenesisFlowEditor::Shutdown()
    {
        ed::DestroyEditor(m_NodeEditorContext);
    }

    void GenesisFlowEditor::Render(std::string UniqueKey)
    {

        RenderNodes(UniqueKey);
    }

    void GenesisFlowEditor::RenderNodes(std::string UniqueKey)
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

        if (m_NodeEditorContext)
        {
            ed::SetCurrentEditor(m_NodeEditorContext);
            ed::GetStyle().NodeRounding = 5.f;

            ed::Begin(UniqueKey.c_str(), {-1, -1});

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

                if (RenderNodeOperation(nodeBuilder, currentIterator.second) == true)
                {
                    this->SendOperationUpdate(currentIterator.second);
                }
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
                    if (ed::AcceptNewItem())
                    {
                        CreateLink(startPinId.Get(), endPinId.Get());
                        m_TriggerCheck = true;
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
                        RemoveLink(deletedLinkId.Get());
                        m_TriggerCheck = true;
                    }
                }
            }
            ed::EndDelete();

            {

                bool actionDelete = ImGui::IsKeyPressed(ImGuiKey_Delete) || (ImGui::IsKeyPressed(ImGuiKey_Backspace) & (!ImGui::GetIO().WantTextInput));

                // if (auto numSelectedLinks = ed::GetSelectedLinks(nullptr, 0); numSelectedLinks > 0)
                //{
                //     ed::LinkId* selectedLinks = new ed::LinkId[numSelectedLinks];
                //     ed::GetSelectedLinks(selectedLinks, numSelectedLinks);

                //    for (int currentSelectedLinkIndex = 0; currentSelectedLinkIndex < numSelectedLinks; currentSelectedLinkIndex++)
                //    {
                //        ed::LinkId selectedLink = selectedLinks[currentSelectedLinkIndex];

                //        if (actionDelete)
                //        {
                //            // m_Links.erase(selectedLink.Get());
                //        }
                //    }
                //}

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
                m_TriggerActionFocusFirstNode = false;

                if (m_Operations.size() > 0)
                {
                    ed::NavigateToContent();
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
                static std::map<std::string, operations::GenesisOperationType> sNewItems = {{"Pattern", operations::GenesisOperationType::FIND_PATTERN},
                                                                                            {"Math", operations::GenesisOperationType::MATH},
                                                                                            {"Debug", operations::GenesisOperationType::DEBUG},
                                                                                            {"Get Value", operations::GenesisOperationType::GET},
                                                                                            {"Rip", operations::GenesisOperationType::RIP},
                                                                                            {"Class Member Variable", operations::GenesisOperationType::OUTPUT_DATA_CLASS_MEMBER_VARIABLE},
                                                                                            {"Class Virtual Function", operations::GenesisOperationType::OUTPUT_DATA_CLASS_VIRTUAL_FUNCTION},
                                                                                            {"Class Non Virtual Function", operations::GenesisOperationType::OUTPUT_DATA_CLASS_NON_VIRTUAL_FUNCTION},
                                                                                            {"Static Function", operations::GenesisOperationType::OUTPUT_DATA_STATIC_FUNCTION},
                                                                                            {"Static Variable", operations::GenesisOperationType::OUTPUT_DATA_STATIC_VARIABLE}};

                ImGui::Text("Create new Node");
                ImGui::Separator();

                for (auto currentIterator : sNewItems)
                {
                    if (ImGui::MenuItem(currentIterator.first.data()))
                    {
                        CreateOperationInFlowFromTypeWithPosition(currentIterator.second, ed::ScreenToCanvas(ImGui::GetMousePos()), true);
                    }
                }

                ImGui::EndPopup();
            }

            ed::Resume();

            if (m_TriggerRestoreStateOfNodes)
            {
                m_TriggerRestoreStateOfNodes = false;

                for (auto currentIterator : m_UninitializedSavedPositionsOfNodes)
                {
                    ed::RestoreNodeState(currentIterator.first);
                    ed::SetNodeZPosition(currentIterator.first, 2.f);
                    ed::SetNodePosition(currentIterator.first, ImVec2(currentIterator.second.first, currentIterator.second.second));
                }
            }

            ed::End();

            ed::SetCurrentEditor(nullptr);
        }
    }

    bool GenesisFlowEditor::RenderNodeOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation)
    {
        return GenesisOperationEditorForNodes::sfRenderOperation(Builder, Operation);
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

        if (Information.m_IsInteractOperation)
        {
            nH += 300.f;
        }

        nH = fmodf(nH, 360.f);

        float h = nH / 360.f;
        float s = 100.f / 100.f;
        float v = 65.f / 100.f;

        ImGui::ColorConvertHSVtoRGB(h, s, v, OutputNormalColor->Value.x, OutputNormalColor->Value.y, OutputNormalColor->Value.z);

        OutputNormalColor->Value.w = 1.f;

        return true;
    }

    // Expanding previous GenesisFlow

    GenesisOperationState* GenesisFlowEditor::CreateOperationState(common::GenesisLoadedFile* LoadedFile)
    {
        GenesisOperationState* operationState = GenesisFlow::CreateOperationState(LoadedFile);

        operationState->SetLogger(&m_Logger);

        return operationState;
    }

    // Obligatory Save/Load stuff

    void GenesisFlowEditor::Reset()
    {
        m_UninitializedSavedPositionsOfNodes.clear();

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
                ImVec2 pos = {};

                uintptr_t currentNodeStateId = reservedBufferGuiStream.Read<uintptr_t>();
                reservedBufferGuiStream.ReadRawIntoPointer(&pos, sizeof(pos));

                m_UninitializedSavedPositionsOfNodes.emplace(currentNodeStateId, std::make_pair(pos.x, pos.y));
            }
        }

        m_TriggerRestoreStateOfNodes = true;

        return res;
    }

    bool GenesisFlowEditor::Export(ash::AshStream* Stream)
    {
        {
            ash::AshStreamExpandableExportBuffer reservedBufferGuiStream = ash::AshStreamExpandableExportBuffer();

            if (m_TriggerRestoreStateOfNodes == false)
            {
                reservedBufferGuiStream.Write<ash::AshSize>(m_Operations.size());

                for (auto currentIterator : m_Operations)
                {
                    ax::NodeEditor::EditorContext* contextBefore = ed::GetCurrentEditor();
                    ed::SetCurrentEditor(m_NodeEditorContext);
                    ImVec2 pos = ed::GetNodePosition(currentIterator.first);
                    ed::SetCurrentEditor(contextBefore);

                    reservedBufferGuiStream.Write<uintptr_t>(currentIterator.first);
                    reservedBufferGuiStream.WriteRawFromPointer(&pos, sizeof(pos));
                }
            }
            else
            {
                reservedBufferGuiStream.Write<ash::AshSize>(m_UninitializedSavedPositionsOfNodes.size());

                for (auto currentIterator : m_UninitializedSavedPositionsOfNodes)
                {
                    ImVec2 pos = {currentIterator.second.first, currentIterator.second.second};

                    reservedBufferGuiStream.Write<uintptr_t>(currentIterator.first);
                    reservedBufferGuiStream.WriteRawFromPointer(&pos, sizeof(pos));
                }
            }

            if (auto res = reservedBufferGuiStream.MakeCopyOfBuffer(); res)
            {
                m_ReservedBufferGui.CopyAshBufferFromPointer(res);
                delete res;
            }
        }

        return GenesisFlow::Export(Stream);
    }

    std::pair<operations::GenesisBaseOperation*, operations::GenesisOperationId> GenesisFlowEditor::CreateOperationInFlowFromType(operations::GenesisOperationType OperationType)
    {
        return CreateOperationInFlowFromType(OperationType, true);
    }

    std::pair<operations::GenesisBaseOperation*, operations::GenesisOperationId> GenesisFlowEditor::CreateOperationInFlowFromType(operations::GenesisOperationType OperationType,
                                                                                                                                  bool BroadcastLiveAction)
    {
        auto res = GenesisFlow::CreateOperationInFlowFromType(OperationType);

        if (res.first != nullptr && BroadcastLiveAction)
        {
            live::GenesisLiveConnectionPacketFlowAction packetFlowAction = live::GenesisLiveConnectionPacketFlowAction();
            packetFlowAction.SetAsActionNodeCreated({.m_OperationType = OperationType, .m_ExpectedNodeIdResult = res.second, .m_X = 0.f, .m_Y = 0.f});
            SendLiveFlowAction(packetFlowAction);
        }

        return std::move(res);
    }

    std::pair<operations::GenesisBaseOperation*, operations::GenesisOperationId> GenesisFlowEditor::CreateOperationInFlowFromTypeWithPosition(operations::GenesisOperationType OperationType,
                                                                                                                                              ImVec2 Position, bool BroadcastLiveAction)
    {
        auto res = GenesisFlow::CreateOperationInFlowFromType(OperationType);

        if (res.first != nullptr && BroadcastLiveAction)
        {
            live::GenesisLiveConnectionPacketFlowAction packetFlowAction = live::GenesisLiveConnectionPacketFlowAction();
            packetFlowAction.SetAsActionNodeCreated({.m_OperationType = OperationType, .m_ExpectedNodeIdResult = res.second, .m_X = Position.x, .m_Y = Position.y});
            SendLiveFlowAction(packetFlowAction);
        }

        ed::EditorContext* contextBefore = ed::GetCurrentEditor();
        ed::SetCurrentEditor(m_NodeEditorContext);
        ed::SetNodePosition(res.second, Position);
        ed::SetCurrentEditor(contextBefore);

        return std::move(res);
    }

    bool GenesisFlowEditor::RemoveOperationFromFlow(operations::GenesisOperationId OperationId)
    {
        return this->RemoveOperationFromFlow(OperationId, true);
    }

    bool GenesisFlowEditor::RemoveOperationFromFlow(operations::GenesisOperationId OperationId, bool BroadcastLiveAction)
    {
        if (GenesisFlow::RemoveOperationFromFlow(OperationId))
        {
            if (BroadcastLiveAction)
            {
                live::GenesisLiveConnectionPacketFlowAction packetFlowAction = live::GenesisLiveConnectionPacketFlowAction();
                packetFlowAction.SetAsActionNodeDeleted(live::GenesisLiveConnectionPacketFlowAction::ActionNodeDeleted{.m_NodeId = OperationId});
                SendLiveFlowAction(packetFlowAction);
            }

            return true;
        }

        return false;
    }

    void GenesisFlowEditor::DoAction(ActionType Action, void* Reserved)
    {
        switch (Action)
        {
        case ActionType::NAVIGATE_TO_CONTENT:
            m_TriggerActionFocusFirstNode = true;
            break;
        }
    }

    void GenesisFlowEditor::CreateLink(uintptr_t FromLinkId, uintptr_t ToLinkId, bool BroadcastLiveAction)
    {
        utils::GenesisPinValue startPinParsed = utils::GenesisPinValue::Unpack(FromLinkId);
        utils::GenesisPinValue endPinParsed = utils::GenesisPinValue::Unpack(ToLinkId);

        if (startPinParsed.m_NodePinType == utils::GenesisPinType::INPUT && endPinParsed.m_NodePinType == utils::GenesisPinType::OUTPUT)
        {
            uintptr_t carry = FromLinkId;
            FromLinkId = ToLinkId;
            ToLinkId = carry;

            startPinParsed = utils::GenesisPinValue::Unpack(FromLinkId);
            endPinParsed = utils::GenesisPinValue::Unpack(ToLinkId);
        }

        if (FromLinkId && ToLinkId && startPinParsed.m_NodePinType == utils::GenesisPinType::OUTPUT && endPinParsed.m_NodePinType == utils::GenesisPinType::INPUT)
        {
            ax::NodeEditor::LinkId linkId = ++m_CounterLinks;

            m_Links.emplace(linkId, std::make_pair(FromLinkId, ToLinkId));
        }

        if (BroadcastLiveAction)
        {
            live::GenesisLiveConnectionPacketFlowAction packetFlowAction = live::GenesisLiveConnectionPacketFlowAction();
            packetFlowAction.SetAsActionLinkCreated(live::GenesisLiveConnectionPacketFlowAction::ActionLinkCreated{.m_FromLinkId = FromLinkId, .m_ToLinkId = ToLinkId});
            SendLiveFlowAction(packetFlowAction);
        }
    }

    void GenesisFlowEditor::RemoveLink(uintptr_t LinkId, bool BroadcastLiveAction)
    {
        if (m_Links.contains(LinkId))
        {
            m_Links.erase(LinkId);

            if (BroadcastLiveAction)
            {
                live::GenesisLiveConnectionPacketFlowAction packetFlowAction = live::GenesisLiveConnectionPacketFlowAction();
                packetFlowAction.SetAsActionLinkDeleted(live::GenesisLiveConnectionPacketFlowAction::ActionLinkDeleted{.m_LinkId = LinkId});
                SendLiveFlowAction(packetFlowAction);
            }
        }
        else
        {
            m_Logger.Log("Error", "Failed to delete link id {} because it is not existing.", LinkId);
        }
    }

    void GenesisFlowEditor::SetNodePosition(uintptr_t NodeId, ImVec2 Position, bool BroadcastLiveAction)
    {
        if (m_Operations.contains(NodeId))
        {
            if (m_TriggerRestoreStateOfNodes)
            {
                if (m_UninitializedSavedPositionsOfNodes.contains(NodeId))
                {
                    m_UninitializedSavedPositionsOfNodes.erase(NodeId);
                }

                m_UninitializedSavedPositionsOfNodes.emplace(NodeId, std::make_pair(Position.x, Position.y));
            }
            else
            {
                ed::EditorContext* contextBefore = ed::GetCurrentEditor();
                ed::SetCurrentEditor(m_NodeEditorContext);
                ed::SetNodePosition(NodeId, Position);
                ed::SetCurrentEditor(contextBefore);
            }
        }

        if (BroadcastLiveAction)
        {
            live::GenesisLiveConnectionPacketFlowAction packetFlowAction = live::GenesisLiveConnectionPacketFlowAction();
            packetFlowAction.SetAsActionSetPositionNode({.m_NodeId = NodeId, .m_X = Position.x, .m_Y = Position.y});
            SendLiveFlowAction(packetFlowAction);
        }
    }

    ash::AshResult GenesisFlowEditor::HandleLiveFlowAction(live::GenesisLiveConnectionPacketFlowAction* Action)
    {
        switch (Action->GetActionType())
        {
        case live::GenesisLiveConnectionPacketFlowAction::ActionType::LINK_CREATED:
        {
            CreateLink(Action->GetAsActionLinkCreated().m_FromLinkId, Action->GetAsActionLinkCreated().m_ToLinkId, false);
            break;
        }
        case live::GenesisLiveConnectionPacketFlowAction::ActionType::LINK_DELETED:
        {
            RemoveLink(Action->GetAsActionLinkDeleted().m_LinkId, false);
            break;
        }
        case live::GenesisLiveConnectionPacketFlowAction::ActionType::SET_POSITION_NODE:
        {
            SetNodePosition(Action->GetAsActionSetPositionNode().m_NodeId, {Action->GetAsActionSetPositionNode().m_X, Action->GetAsActionSetPositionNode().m_Y}, false);
            break;
        }
        case live::GenesisLiveConnectionPacketFlowAction::ActionType::NODE_CREATED:
        {
            auto res = CreateOperationInFlowFromTypeWithPosition(Action->GetAsActionNodeCreated().m_OperationType, {Action->GetAsActionNodeCreated().m_X, Action->GetAsActionNodeCreated().m_Y}, false);

            if (res.second != Action->GetAsActionNodeCreated().m_ExpectedNodeIdResult)
            {
                return ash::AshResult(false, "Unexpected node id.");
            }

            break;
        }
        case live::GenesisLiveConnectionPacketFlowAction::ActionType::NODE_DELETED:
        {
            RemoveOperationFromFlow(Action->GetAsActionNodeDeleted().m_NodeId, false);
            break;
        }
        default:
            return ash::AshResult(false, "Action is not implemented.");
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisFlowEditor::HandleOperationUpdate(live::GenesisLiveConnectionPacketOperationUpdate* Action)
    {
        if (m_Operations.contains(Action->GetOperationId()))
        {
            ash::AshBuffer buffer = Action->GetBuffer();
            ash::AshStreamStaticBuffer stream = ash::AshStreamStaticBuffer(&buffer, ash::AshStreamMode::READ);

            m_Operations.at(Action->GetOperationId())->Import(&stream);
        }
        else
        {
            return ash::AshResult(false, "Received update on non-existing operation.");
        }

        return ash::AshResult(true);
    }

    ash::AshResult GenesisFlowEditor::SendLiveFlowAction(live::GenesisLiveConnectionPacketFlowAction* Action)
    {
        if (m_Live != nullptr && m_LiveFlowName.empty() == false)
        {
            Action->SetFlowTargetName(m_LiveFlowName);
            return m_Live->BroadcastPacketToPeers(Action);
        }

        // Ignore if live is not present.
        return ash::AshResult(m_Live == nullptr, m_LiveFlowName.empty() ? "Live Flow Name not assigned" : "");
    }

    ash::AshResult GenesisFlowEditor::SendOperationUpdate(operations::GenesisBaseOperation* Operation)
    {
        if (m_Live == nullptr || m_LiveFlowName.empty())
        {
            return ash::AshResult(true);
        }

        live::GenesisLiveConnectionPacketOperationUpdate packet = live::GenesisLiveConnectionPacketOperationUpdate();

        packet.SetOperationId(Operation->GetOperationId());
        packet.SetFlowName(m_LiveFlowName);

        {
            auto exportStream = ash::AshStreamExpandableExportBuffer();
            Operation->Export(&exportStream);

            if (auto exportBuffer = exportStream.MakeCopyOfBuffer())
            {
                packet.SetBuffer(*exportBuffer);
                delete exportBuffer;
            }
        }

        return m_Live->BroadcastPacketToPeers(&packet);
    }

} // namespace genesis::editor
