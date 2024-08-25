#include "GenesisMerge/GenesisMergeBundle.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "GenesisMerge/GenesisMergeFlow.hpp"
#include "GenesisShared/GenesisBundle.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "imgui.h"
#include <algorithm>
#include <vector>

namespace genesis::merge
{

    /*
     * Sorry this code is kinda shit.
     */

    GenesisBundleMerge::GenesisBundleMerge(ash::AshLogger& Logger, GenesisBundle* Base, GenesisBundle* Local, GenesisBundle* Remote) : m_Logger(Logger), m_Flows(), m_NeedsToResolveFlows(false)
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
        if (ImGui::BeginTable("##MergeResolveTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable, {-1, -1}))
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
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(currentIterator.first.data());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(GenesisFlowMerge::sfGetFlowStatusAsString(currentIterator.second->GetLocalStatus()).data());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(GenesisFlowMerge::sfGetFlowStatusAsString(currentIterator.second->GetRemoteStatus()).data());
                ImGui::TableNextColumn();

                if(currentIterator.second->IsNeededToBeResolvedManually())
                {
                    ImGui::Text("Unresolved");
                }
                else
                {
                    ImGui::Text("Resolved");
                }
            }

            ImGui::EndTable();
        }
    }

} // namespace genesis::merge
