#include "GenesisShared/GenesisBundle.hpp"
#include "Ash/AshResult.h"
#include "Ash/AshStream.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisFlow.hpp"
#include "fmt/format.h"
#include <cstddef>

namespace genesis
{

    GenesisFlow* GenesisBundle::sfDefaultFlowFactory(void*)
    {
        return new GenesisFlow();
    }

    GenesisBundle::GenesisBundle(sdFlowFactory FlowFactory) : m_FlowFactory(FlowFactory), m_Flows(), m_ReservedFactoryValue()
    {
    }

    GenesisBundle::~GenesisBundle()
    {
        this->Reset();
    }

    ash::AshResult GenesisBundle::CreateFlow(std::string FlowName)
    {
        return AddFlow(FlowName, m_FlowFactory(m_ReservedFactoryValue));
    }

    ash::AshResult GenesisBundle::AddFlow(std::string FlowName, GenesisFlow* Flow)
    {
        if (m_Flows.contains(FlowName))
        {
            return ash::AshResult(false, "Already containing a flow with this name");
        }

        m_Flows.emplace(FlowName, Flow);

        return ash::AshResult(true);
    }

    ash::AshResult GenesisBundle::RemoveFlow(std::string FlowName, bool FreeFlow)
    {
        if (m_Flows.contains(FlowName) == false)
        {
            return ash::AshResult(false, "No flow with such name.");
        }

        if (auto flow = m_Flows.at(FlowName))
        {
            if (FreeFlow)
            {
                delete flow;
            }
        }

        m_Flows.erase(FlowName);
        return ash::AshResult(true);
    }

    ash::AshResult GenesisBundle::ProcessBundle(output::GenesisOutputData* OutputData, common::GenesisLoadedFile* LoadedFile)
    {
        for (auto currentIterator : m_Flows)
        {
            if (auto res = currentIterator.second->ProcessFlow(OutputData, LoadedFile); res.HasError())
            {
                return ash::AshResult(false, fmt::format("Flow {} failed. {}", currentIterator.first, res.GetMessage()));
            }
        }

        return ash::AshResult(true);
    }

    void GenesisBundle::Reset()
    {
        for (auto currentIterator : m_Flows)
        {
            delete currentIterator.second;
        }

        m_Flows.clear();
    }

    bool GenesisBundle::Import(ash::AshStream* Stream)
    {
        this->Reset();

        size_t flowsSize = Stream->Read<ash::AshSize>();

        for (size_t currentIndex = 0; currentIndex < flowsSize; currentIndex++)
        {
            ash::objects::AshAsciiString currentFlowKey = ash::objects::AshAsciiString();
            GenesisFlow* currentFlowValue = m_FlowFactory(m_ReservedFactoryValue);

            if (currentFlowKey.Import(Stream) == false)
            {
                std::cout << "Failed to read currentFlowKey" << std::endl;
                return false;
            }

            if (currentFlowValue->Import(Stream) == false)
            {
                std::cout << "Failed to read currentFlowValue" << std::endl;
                return false;
            }

            m_Flows.emplace(currentFlowKey.GetText(), currentFlowValue);
        }

        return Stream->IsOkay();
    }

    bool GenesisBundle::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshSize>(m_Flows.size());

        for (auto currentIterator : m_Flows)
        {
            ash::objects::AshAsciiString(currentIterator.first).Export(Stream);
            currentIterator.second->Export(Stream);
        }

        return Stream->IsOkay();
    }

} // namespace genesis
