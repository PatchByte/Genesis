#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisNodeBuilder.hpp"
#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "GenesisShared/GenesisPinTracker.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace ed = ax::NodeEditor;

namespace genesis::editor
{

    GenesisFlowEditor::GenesisFlowEditor() : GenesisFlow(), m_Logger("GuiLogger", {}), m_LogBox(), m_DockSpaceId(0), m_DockSpaceHasBeenBuilt(false), m_TriggerCheck(false)
    {
        AddOperationToFlow(new operations::GenesisFindPatternOperation("E8 ? ? ? ? 90"));
        AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 6));
        AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 2));

        m_Logger.AddLoggerPassage(m_LogBox.CreatePassage());
    }

    GenesisFlowEditor::~GenesisFlowEditor()
    {
    }

    void GenesisFlowEditor::Initialize()
    {
        ed::Config nodeEditorConfig = ed::Config();

        nodeEditorConfig.EnableSmoothZoom = true;

        m_NodeEditorContext = ed::CreateEditor(&nodeEditorConfig);
    }

    void GenesisFlowEditor::Shutdown()
    {
        ed::DestroyEditor(m_NodeEditorContext);
    }

    void GenesisFlowEditor::Render()
    {
        if (m_DockSpaceId == 0)
        {
            m_DockSpaceId = ImGui::GetID("dockSpace");
        }

        if (m_DockSpaceHasBeenBuilt == false)
        {
            ImGui::DockBuilderRemoveNode(m_DockSpaceId);
            ImGui::DockBuilderAddNode(m_DockSpaceId);

            ImGui::DockBuilderSplitNode(m_DockSpaceId, ImGuiDir_Down, 0.3f, &m_DockLogWindow, &m_DockNodeWindow);
            ImGui::DockBuilderDockWindow("Nodes", m_DockNodeWindow);
            ImGui::DockBuilderDockWindow("Log", m_DockLogWindow);

            ImGui::DockBuilderFinish(m_DockSpaceId);

            m_DockSpaceHasBeenBuilt = true;
        }

        ImGui::DockSpace(m_DockSpaceId, ImVec2(-1, -1),
                         ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_HiddenTabBar);

        if (ImGui::Begin("Nodes", nullptr, ImGuiWindowFlags_MenuBar))
        {
            RenderNodes();
        }
        ImGui::End();

        if (ImGui::Begin("Log"))
        {
            m_LogBox.Render();
        }
        ImGui::End();
    }

    void GenesisFlowEditor::RenderNodes()
    {
        if (m_TriggerCheck)
        {
            m_LogBox.Clear();

            if (auto res = CheckIfFlowIsRunnable(); res.HasError())
            {
                m_Logger.Log("Error", "Failed to check. {}", res.GetMessage());
            }
            else
            {
                m_Logger.Log("Info", "Node graph is working.");
            }

            m_TriggerCheck = false;
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::MenuItem("Organize"))
            {
            }

            ImGui::EndMenuBar();
        }

        ed::SetCurrentEditor(m_NodeEditorContext);

        ed::GetStyle().NodeRounding = 5.f;

        ed::Begin("MainEditor", ImVec2(-1, -1));

        utils::GenesisNodeBuilder nodeBuilder = utils::GenesisNodeBuilder();

        for (auto currentIterator : m_Operations)
        {
            ImColor normalColor = ImColor(), brightColor = ImColor();

            sfGetColorForOperationInformation(currentIterator.second->GetOperationInformation(), &normalColor, &brightColor);

            // ImNodes::PushColorStyle(ImNodesCol_TitleBar, normalColor);
            // ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, brightColor);
            // ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, brightColor);

            ImGui::PushID(currentIterator.first);

            nodeBuilder.Begin(currentIterator.first);

            nodeBuilder.Header("", normalColor);
            ImGui::BeginGroup();
            ImGui::Text("%s (%i)", currentIterator.second->GetOperationName().data(), currentIterator.first);
            ImGui::Dummy({ 0.f, 2.f });
            ImGui::EndGroup();
            nodeBuilder.EndHeader();

            RenderNodeOperation(nodeBuilder, currentIterator.second);
            nodeBuilder.End();

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

        ed::End();

        ed::SetCurrentEditor(nullptr);
    }

    void GenesisFlowEditor::RenderNodeOperation(utils::GenesisNodeBuilder& Builder, operations::GenesisBaseOperation* Operation)
    {
        GenesisOperationEditorForNodes::sfRenderOperation(Builder, Operation);
    }

    bool GenesisFlowEditor::sfGetColorForOperationInformation(const operations::GenesisOperationInformation& Information, ImColor* OutputNormalColor, ImColor* OutputBrightColor)
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

        v = 89.f / 100.f;

        ImGui::ColorConvertHSVtoRGB(h, s, v, OutputBrightColor->Value.x, OutputBrightColor->Value.y, OutputBrightColor->Value.z);

        OutputNormalColor->Value.w = 1.f;
        OutputBrightColor->Value.w = 1.f;

        return true;
    }

} // namespace genesis::editor