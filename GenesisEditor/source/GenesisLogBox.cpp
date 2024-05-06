#include "GenesisEditor/GenesisLogBox.hpp"
#include "AshLogger/AshLoggerPassage.h"
#include "AshLogger/AshLoggerTag.h"
#include "fmt/format.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include <utility>

namespace genesis::utils
{

    GenesisLogBox::GenesisLogBox() : m_Logs(), m_TagColors()
    {
        Initialize();
    }

    GenesisLogBox::~GenesisLogBox()
    {
    }

    void GenesisLogBox::Initialize()
    {
        auto warningTag = ash::AshLoggerTag("Warning");
        auto errorTag = ash::AshLoggerTag("Error");

        m_TagColors.emplace(warningTag.GetShortTag(), std::make_pair(warningTag, ImColor(255, 160, 0)));
        m_TagColors.emplace(errorTag.GetShortTag(), std::make_pair(errorTag, ImColor(230, 74, 25)));
    }

    void GenesisLogBox::Render()
    {
        // if(ImGui::BeginTable("##LogTable", 2, ImGuiTableFlags_RowBg))
        //{

        //    for(auto currentIterator : m_Logs)
        //    {
        //        ImGui::TableNextRow();
        //        ImGui::TableSetColumnIndex(0);
        //        ImGui::Text("%s", currentIterator.first.GetPrefix().data());
        //        ImGui::TableNextColumn();
        //        ImGui::Text("%s", currentIterator.second.data());
        //    }

        //    ImGui::EndTable();
        //}

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImNodes::GetStyle().Colors[ImNodesCol_GridBackground]);

        if (ImGui::BeginChild("LogChild", ImVec2(-1, -1)))
        {
            ImGui::Dummy(ImVec2(0.f, 1.f));

            for (auto currentIterator : m_Logs)
            {
                ImGui::Columns(2);
                ImGui::SetColumnWidth(-1, ImGui::GetWindowWidth() * 0.2);
                ImGui::Text("%s", currentIterator.first.GetPrefix().data());
                ImGui::NextColumn();
                ImGui::Text("%s", currentIterator.second.data());
            }

            ImGui::EndChild();
        }

        ImGui::PopStyleColor();
    }

    void GenesisLogBox::Clear()
    {
        m_Logs.clear();
    }

    ash::AshLoggerPassage* GenesisLogBox::CreatePassage()
    {
        return new ash::AshLoggerFunctionPassage([this](ash::AshLoggerDefaultPassage* This, ash::AshLoggerTag Tag, std::string Format, fmt::format_args Args, std::string FormattedString) -> void
                                                 { m_Logs.push_back(std::make_pair(Tag, fmt::vformat(Format, Args))); });
    }

} // namespace genesis::utils