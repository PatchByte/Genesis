#include "GenesisShared/GenesisBundle.hpp"
#include "Ash/AshStream.h"
#include "AshObjects/AshString.h"
#include "GenesisShared/GenesisFlow.hpp"
#include <cstddef>

namespace genesis
{

    GenesisBundle::GenesisBundle(sdFlowFactory FlowFactory):
        m_FlowFactory(FlowFactory),
        m_Flows(),
        m_ReservedFactoryValue()
    {}

    GenesisBundle::~GenesisBundle()
    {
        this->Reset();
    }

    void GenesisBundle::Reset()
    {
        for(auto currentIterator : m_Flows)
        {
            delete currentIterator.second;
        }

        m_Flows.clear();
    }

    bool GenesisBundle::Import(ash::AshStream* Stream)
    {
        this->Reset();

        size_t flowsSize = Stream->Read<size_t>();

        for(size_t currentIndex = 0; currentIndex < flowsSize; currentIndex++)
        {
            ash::objects::AshAsciiString currentFlowKey = ash::objects::AshAsciiString();
            GenesisFlow* currentFlowValue = m_FlowFactory(m_ReservedFactoryValue);
        
            if(currentFlowKey.Import(Stream) == false)
            {
                std::cout << "Failed to read currentFlowKey" << std::endl;
                return false;
            }

            if(currentFlowValue->Import(Stream) == false)
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
        Stream->Write<size_t>(m_Flows.size());

        for(auto currentIterator : m_Flows)
        {
            ash::objects::AshAsciiString(currentIterator.first).Export(Stream);
            currentIterator.second->Export(Stream);
        }
        
        return Stream->IsOkay();
    }
    
}