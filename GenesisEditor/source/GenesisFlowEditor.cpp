#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include <algorithm>
#include <cmath>

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

        if (ImGui::Begin("Nodes"))
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
        if(m_TriggerCheck)
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

        ImNodes::BeginNodeEditor();

        for (auto currentIterator : m_Operations)
        {
            ImColor normalColor = ImColor(), brightColor = ImColor();

            sfGetColorForOperationInformation(currentIterator.second->GetOperationInformation(), &normalColor, &brightColor);

            ImNodes::PushColorStyle(ImNodesCol_TitleBar, normalColor);
            ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, brightColor);
            ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, brightColor);

            ImNodes::BeginNode(currentIterator.first);
            RenderNodeOperation(currentIterator.second);
            ImNodes::EndNode();

            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
        }

        for (int i = 0; i < m_Links.size(); ++i)
        {
            const std::pair<int, int> p = m_Links[i];
            // in this case, we just use the array index of the link
            // as the unique identifier
            ImNodes::Link(i, p.first, p.second);
        }

        if (ImGui::IsWindowFocused())
        {
            bool hasPressedDeleteKey = ImGui::IsKeyPressed(ImGuiKey_Delete);
            bool hasPressedSpecialDebugKey = ImGui::IsKeyPressed(ImGuiKey_PageDown);

            if (int numSelectedLinks = ImNodes::NumSelectedLinks(); numSelectedLinks > 0)
            {
                int* selectedLinks = new int[numSelectedLinks];

                ImNodes::GetSelectedLinks(selectedLinks);

                if (hasPressedDeleteKey)
                {
                    for (int currentSelectedLinkIndex = 0; currentSelectedLinkIndex < numSelectedLinks; currentSelectedLinkIndex++)
                    {
                        m_Links.erase(selectedLinks[currentSelectedLinkIndex]);
                    }
                }

                delete[] selectedLinks;
            }

            if (int numSelectedNodes = ImNodes::NumSelectedNodes(); numSelectedNodes > 0)
            {
                int* selectedNodes = new int[numSelectedNodes];

                ImNodes::GetSelectedNodes(selectedNodes);

                if (hasPressedDeleteKey)
                {
                    for (int currentSelectedNodeIndex = 0; currentSelectedNodeIndex < numSelectedNodes; currentSelectedNodeIndex++)
                    {
                        this->RemoveOperationFromFlow(selectedNodes[currentSelectedNodeIndex]);
                    }

                    m_TriggerCheck = true;
                }

                if (hasPressedSpecialDebugKey)
                {
                    for (int currentSelectedNodeIndex = 0; currentSelectedNodeIndex < numSelectedNodes; currentSelectedNodeIndex++)
                    {
                        for (operations::GenesisOperationId currentOperationId : this->CollectAllNodeLinkIdsToOtherNodesFromNode(selectedNodes[currentSelectedNodeIndex]))
                        {
                            ImNodes::SelectNode(currentOperationId);
                        }
                    }
                }

                delete[] selectedNodes;
            }
        }

        ImNodes::EndNodeEditor();

        {
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                m_Links.emplace(++m_CounterLinks, std::make_pair(start_attr, end_attr));
                m_TriggerCheck = true;
            }
        }

        {
            int link_id;
            if (ImNodes::IsLinkDestroyed(&link_id))
            {
                if (m_Links.contains(link_id))
                {
                    printf("Destroying: %i\n", link_id);
                    m_Links.erase(link_id);
                }
                else
                {
                    printf("Destroying failed: %i\n", link_id);
                }

                m_TriggerCheck = true;
            }
        }
    }

    void GenesisFlowEditor::RenderNodeOperation(operations::GenesisBaseOperation* Operation)
    {
        ImNodes::BeginNodeTitleBar();
        ImGui::Text("%s %i", Operation->GetOperationName().data(), Operation->GetOperationId());
        ImNodes::EndNodeTitleBar();

        GenesisOperationEditorForNodes::sfRenderOperation(Operation);
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