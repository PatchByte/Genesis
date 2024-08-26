#include "GenesisMerge/GenesisMergeBundle.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "GenesisMerge/GenesisMergeFlow.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "fmt/format.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <vector>

namespace genesis::merge
{

    /*
     * Sorry this code is kinda shit.
     */

    GenesisBundleMerge::GenesisBundleMerge(ash::AshLogger& Logger, GenesisBundle* Base, GenesisBundle* Local, GenesisBundle* Remote) : m_Logger(Logger), m_Flows()
    {
        std::vector<std::string> flowNames = {};

        for (GenesisBundle* currentBundle : {Base, Local, Remote})
        {
            for (auto currentFlowIterator : currentBundle->GetFlows())
            {
                if (std::find(flowNames.begin(), flowNames.end(), currentFlowIterator.first) == flowNames.end())
                {
                    flowNames.push_back(currentFlowIterator.first);
                }
            }
        }

        for (std::string currentFlowName : flowNames)
        {
            if (m_Flows.contains(currentFlowName))
            {
                m_Logger.Log("Error", "Flow already containing, yet tried to add. \"{}\"", currentFlowName);
            }
            else
            {
                auto currentFlowMerge = new GenesisFlowMerge();

                if (auto status = currentFlowMerge->Process(Base->GetFlow(currentFlowName), Local->GetFlow(currentFlowName), Remote->GetFlow(currentFlowName)); status.HasError())
                {
                    m_Logger.Log("Error", "Error while processing flows of {}. {}", currentFlowName, status.GetMessage());
                    delete currentFlowMerge;
                    continue;
                }

                m_Flows.emplace(currentFlowName, currentFlowMerge);
            }
        }
    }

    GenesisBundleMerge::~GenesisBundleMerge()
    {
        sdFlows flows = m_Flows;
        m_Flows.clear();

        for (auto currentFlow : flows)
        {
            delete currentFlow.second;
        }
    }

    ash::AshResult GenesisBundleMerge::IsMergeable()
    {
        for (auto currentIterator : m_Flows)
        {
            if (currentIterator.second->IsResolved() == false)
            {
                return ash::AshResult(false, fmt::format("Merge issue for Flow \"{}\" has not been resolved!", currentIterator.first));
            }
        }

        return ash::AshResult(true);
    }

    ash::AshCustomResult<ash::AshBuffer*> GenesisBundleMerge::Serialize()
    {
        return ash::AshCustomResult<ash::AshBuffer*>(false);
    }

    GenesisFlow* GenesisBundleMerge::sfFactory(void* Reserved)
    {
        return new GenesisFlow();
    }

    void GenesisBundleMerge::Render()
    {
        if (ImGui::BeginTable("##MergeResolveTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable,
                              {-1, ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Merge", NULL, true).y + ImGui::GetStyle().FramePadding.y * 4.f)}))
        {
            // Headers

            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Name");
            ImGui::TableNextColumn();
            ImGui::Text("Local");
            ImGui::TableNextColumn();
            ImGui::Text("Remote");
            ImGui::TableNextColumn();
            ImGui::Text("Resolvement");

            // Entries

            for (auto currentIterator : m_Flows)
            {
                ImGui::PushID(ImGui::GetID(currentIterator.first.data()));
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                if (currentIterator.second->IsNeededToBeResolvedManually())
                {
                    ImGui::Selectable(currentIterator.first.data(), false, ImGuiSelectableFlags_SpanAllColumns);

                    if (ImGui::BeginPopupContextItem(currentIterator.first.data()))
                    {
                        if (ImGui::MenuItem("Local"))
                        {
                            currentIterator.second->SetManualResolvementStatus(GenesisFlowMerge::ResolvementStatus::TAKE_LOCAL);
                        }

                        if (ImGui::MenuItem("Remote"))
                        {
                            currentIterator.second->SetManualResolvementStatus(GenesisFlowMerge::ResolvementStatus::TAKE_REMOTE);
                        }

                        ImGui::EndPopup();
                    }
                }
                else
                {
                    ImGui::TextUnformatted(currentIterator.first.data());
                }

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(GenesisFlowMerge::sfGetFlowStatusAsString(currentIterator.second->GetLocalStatus()).data());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(GenesisFlowMerge::sfGetFlowStatusAsString(currentIterator.second->GetRemoteStatus()).data());
                ImGui::TableNextColumn();

                if (currentIterator.second->IsNeededToBeResolvedManually())
                {
                    switch (currentIterator.second->GetManualResolvementStatus())
                    {
                    case GenesisFlowMerge::ResolvementStatus::UNRESOLVED:
                        ImGui::Text("Unresolved");
                        break;
                    case GenesisFlowMerge::ResolvementStatus::TAKE_REMOTE:
                        ImGui::Text("Taking Remote");
                        break;
                    case GenesisFlowMerge::ResolvementStatus::TAKE_LOCAL:
                        ImGui::Text("Taking Local");
                        break;
                    }
                }
                else
                {
                    ImGui::Text("Resolved");
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        {
            auto isMergeable = this->IsMergeable();

            if (isMergeable.HasError())
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            ImGui::Button("Merge", {-1, -1});

            if (isMergeable.HasError())
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();

                if (ImGui::BeginItemTooltip())
                {
                    ImGui::TextUnformatted(isMergeable.GetMessage().data());

                    ImGui::EndTooltip();
                }
            }
        }
    }

} // namespace genesis::merge
