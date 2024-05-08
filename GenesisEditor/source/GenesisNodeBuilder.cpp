#include "GenesisEditor/GenesisNodeBuilder.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"

namespace ed = ax::NodeEditor;

namespace genesis::utils
{

    GenesisNodeBuilder::GenesisNodeBuilder(ImTextureID texture, int textureWidth, int textureHeight)
        : m_HeaderTextureId(texture), m_HeaderTextureWidth(textureWidth), m_HeaderTextureHeight(textureHeight), m_CurrentNodeId(0), m_CurrentStage(Stage::Invalid), m_HasHeader(false)
    {
    }

    void GenesisNodeBuilder::Begin(ed::NodeId id)
    {
        m_HasHeader = false;
        m_HeaderTitle = "";
        m_HeaderMin = m_HeaderMax = ImVec2();

        ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

        ed::BeginNode(id);

        ImGui::PushID(id.AsPointer());
        m_CurrentNodeId = id;

        SetStage(Stage::Begin);
    }

    void GenesisNodeBuilder::End()
    {
        m_HeaderMax.x = std::max(m_HeaderMax.x, ImGui::GetItemRectMax().x);

        SetStage(Stage::End);

        ed::EndNode();


        if (ImGui::IsItemVisible())
        {
            auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);

            auto drawList = ed::GetNodeBackgroundDrawList(m_CurrentNodeId);

            const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 1.f;

            auto headerColor = IM_COL32(0, 0, 0, alpha) | (m_HeaderColor & IM_COL32(255, 255, 255, 0));
            if ((m_HeaderMax.x > m_HeaderMin.x) && (m_HeaderMax.y > m_HeaderMin.y))
            {
                const auto uv = ImVec2((m_HeaderMax.x - m_HeaderMin.x) / (float)(4.0f * m_HeaderTextureWidth), (m_HeaderMax.y - m_HeaderMin.y) / (float)(4.0f * m_HeaderTextureHeight));

                drawList->AddRectFilled(m_HeaderMin - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth), m_HeaderMax + ImVec2(8 - halfBorderWidth, 0), m_HeaderColor, ax::NodeEditor::GetStyle().NodeRounding, ImDrawFlags_RoundCornersTop);
                if (true)
                {
                    drawList->AddLine(ImVec2(m_HeaderMin.x - (8 - halfBorderWidth), m_HeaderMax.y - 0.5f), ImVec2(m_HeaderMax.x + (8 - halfBorderWidth), m_HeaderMax.y - 0.5f),
                                      ImColor(ed::GetStyle().Colors[ed::StyleColor_NodeBorder]), 1.4f);
                }

                drawList->AddText((m_HeaderMin - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth)) + (m_HeaderMax / 2) - (ImGui::CalcTextSize(m_HeaderTitle.data()) / 2), ImColor(255, 255, 255), m_HeaderTitle.data());
            }
        }

        m_CurrentNodeId = 0;

        ImGui::PopID();

        ed::PopStyleVar();

        SetStage(Stage::Invalid);
    }

    void GenesisNodeBuilder::Header(std::string Title, const ImVec4& color)
    {
        m_HeaderTitle = Title;
        m_HeaderColor = ImColor(color);
        
        SetStage(Stage::Header);
    }

    void GenesisNodeBuilder::EndHeader()
    {
        SetStage(Stage::Content);

        ImGui::Dummy({ 0.f, 2.f });
    }

    void GenesisNodeBuilder::Input(ed::PinId id)
    {
        if (m_CurrentStage == Stage::Begin)
            SetStage(Stage::Content);

        const auto applyPadding = (m_CurrentStage == Stage::Input);

        SetStage(Stage::Input);

        // if (applyPadding)
        //     ImGui::Spring(0);

        Pin(id, ed::PinKind::Input);

    }

    void GenesisNodeBuilder::EndInput()
    {
        EndPin();
    }

    void GenesisNodeBuilder::Middle()
    {
        if (m_CurrentStage == Stage::Begin)
            SetStage(Stage::Content);

        SetStage(Stage::Middle);
    }

    void GenesisNodeBuilder::Output(ed::PinId id)
    {
        if (m_CurrentStage == Stage::Begin)
            SetStage(Stage::Content);

        const auto applyPadding = (m_CurrentStage == Stage::Output);

        SetStage(Stage::Output);

        // if (applyPadding)
        //     ImGui::Spring(0);

        Pin(id, ed::PinKind::Output);
    }

    void GenesisNodeBuilder::EndOutput()
    {
        EndPin();
    }

    bool GenesisNodeBuilder::SetStage(Stage stage)
    {
        if (stage == m_CurrentStage)
            return false;
            
        m_HeaderMax.x = std::max(m_HeaderMax.x, ImGui::GetItemRectMax().x);

        auto oldStage = m_CurrentStage;
        m_CurrentStage = stage;

        ImVec2 cursor;
        switch (oldStage)
        {
        case Stage::Begin:
            break;

        case Stage::Header:
            m_HeaderMin = ImGui::GetItemRectMin();
            m_HeaderMax = ImGui::GetItemRectMax();

            // spacing between header and content
            // ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

            break;

        case Stage::Content:
            break;

        case Stage::Input:
            ed::PopStyleVar(2);

            // ImGui::Spring(1, 0);
            // ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::Middle:
            // ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::Output:
            ed::PopStyleVar(2);

            // ImGui::Spring(1, 0);
            // ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::End:
            break;

        case Stage::Invalid:
            break;
        }

        switch (stage)
        {
        case Stage::Begin:
            // ImGui::BeginVertical("node");
            break;

        case Stage::Header:
            m_HasHeader = true;

            // ImGui::BeginHorizontal("header");
            break;

        case Stage::Content:
            // if (oldStage == Stage::Begin)
            //     ImGui::Spring(0);

            // ImGui::BeginHorizontal("content");
            // ImGui::Spring(0, 0);
            break;

        case Stage::Input:
            // ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

            ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
            ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

            // if (!m_HasHeader)
            //     ImGui::Spring(1, 0);
            break;

        case Stage::Middle:
            // ImGui::Spring(1);
            // ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
            break;

        case Stage::Output:
            // if (oldStage == Stage::Middle || oldStage == Stage::Input)
            //     ImGui::Spring(1);
            // else
            //     ImGui::Spring(1, 0);
            // ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

            ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
            ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

            // if (!m_HasHeader)
            //     ImGui::Spring(1, 0);
            break;

        case Stage::End:
            // if (oldStage == Stage::Input)
            //     ImGui::Spring(1, 0);
            // if (oldStage != Stage::Begin)
            //     ImGui::EndHorizontal();
            m_ContentMin = ImGui::GetItemRectMin();
            m_ContentMax = ImGui::GetItemRectMax();

            // ImGui::Spring(0);
            // ImGui::EndVertical();
            m_NodeMin = ImGui::GetItemRectMin();
            m_NodeMax = ImGui::GetItemRectMax();
            break;

        case Stage::Invalid:
            break;
        }

        return true;
    }

    void GenesisNodeBuilder::Pin(ed::PinId id, ed::PinKind kind)
    {
        ed::BeginPin(id, kind);
    }

    void GenesisNodeBuilder::EndPin()
    {
        ed::EndPin();

        // #debug
        // ImGui::GetWindowDrawList()->AddRectFilled(
        //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 64));
    }

} // namespace genesis::utils