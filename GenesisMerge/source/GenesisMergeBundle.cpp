#include "GenesisMerge/GenesisMergeBundle.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "GenesisMerge/GenesisMergeFlow.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "fmt/format.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <cstdlib>
#include <vector>

namespace genesis::merge
{

    /*
     * Sorry this code is kinda shit.
     */

    GenesisBundleMerge::GenesisBundleMerge(ash::AshLogger& Logger, GenesisBundle* Base, GenesisBundle* Local, GenesisBundle* Remote, std::filesystem::path Merged)
        : m_Logger(Logger), m_Flows(), m_Merged(Merged)
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
        GenesisBundle* bundleMerged = new GenesisBundle();

        for (auto currentIterator : m_Flows)
        {
            auto resolvedStatus = currentIterator.second->GetResolvedFlowStatus();

            if (resolvedStatus == GenesisFlowMerge::FlowHistorialStatus::DELETED)
            {
                continue;
            }

            if (bundleMerged->CreateFlow(currentIterator.first).HasError())
            {
                m_Logger.Log("Error", "Failed to create flow \"{}\"", currentIterator.first);
                std::exit(-1);
            }

            ash::AshStreamStaticBuffer flowMergedStream = ash::AshStreamStaticBuffer(currentIterator.second->GetResolvedBuffer(), ash::AshStreamMode::READ);

            GenesisFlow* flowMerged = bundleMerged->GetFlow(currentIterator.first);

            if (flowMerged->Import(&flowMergedStream) == false || resolvedStatus == GenesisFlowMerge::FlowHistorialStatus::INVALID)
            {
                delete flowMerged;

                m_Logger.Log("Error", "Failed to import flow \"{}\"", currentIterator.first);
                std::exit(-1);
            }
        }

        ash::AshStreamExpandableExportBuffer bundleMergedStream = ash::AshStreamExpandableExportBuffer();

        if (bundleMerged->Export(&bundleMergedStream) == false)
        {
            m_Logger.Log("Error", "Failed to export merged bundle.");
            std::exit(-1);
        }

        delete bundleMerged;

        return ash::AshCustomResult<ash::AshBuffer*>(true).AttachResult(bundleMergedStream.MakeCopyOfBuffer());
    }

    GenesisFlow* GenesisBundleMerge::sfFactory(void* Reserved)
    {
        return new GenesisFlow();
    }

    void GenesisBundleMerge::Render()
    {
        if (ImGui::BeginTable("##MergeResolveTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
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
                            currentIterator.second->SetManualResolvementStatus(GenesisFlowMerge::FlowManualResolvementStatus::TAKE_LOCAL);
                        }

                        if (ImGui::MenuItem("Remote"))
                        {
                            currentIterator.second->SetManualResolvementStatus(GenesisFlowMerge::FlowManualResolvementStatus::TAKE_REMOTE);
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
                ImGui::Text("%s", GenesisFlowMerge::sfGetResolvementStatusAsLabel(currentIterator.second->GetManualResolvementStatus()).data());

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

} // namespace genesis::merge
