#include "GenesisShared/GenesisOutput.hpp"
#include <vector>

namespace genesis::output
{

    ash::AshResult GenesisOutputClass::AddVirtualFunction(GenesisOutputClassVirtualFunction VirtualFunction)
    {
        if (m_VirtualFunctions.contains(VirtualFunction.GetName()))
        {
            return ash::AshResult(false);
        }

        m_VirtualFunctions.emplace(VirtualFunction.GetName(), VirtualFunction);

        return ash::AshResult(true);
    }

    ash::AshResult GenesisOutputClass::AddNonVirtualFunction(GenesisOutputClassNonVirtualFunction NonVirtualFunction)
    {
        if (m_NonVirtualFunctions.contains(NonVirtualFunction.GetName()))
        {
            return ash::AshResult(false);
        }

        m_NonVirtualFunctions.emplace(NonVirtualFunction.GetName(), NonVirtualFunction);

        return ash::AshResult(true);
    }

    ash::AshResult GenesisOutputClass::AddMember(GenesisOutputClassMember Member)
    {
        if (m_Members.contains(Member.GetName()))
        {
            return ash::AshResult(false);
        }

        m_Members.emplace(Member.GetName(), Member);

        return ash::AshResult(true);
    }

    // GenesisOutputData

    GenesisOutputData::GenesisOutputData()
    {
    }

    GenesisOutputData::~GenesisOutputData()
    {
        for (auto currentIterator : m_Classes)
        {
            delete currentIterator.second;
        }

        m_Classes.clear();
    }

    GenesisOutputClass* GenesisOutputData::GetOrCreateClass(std::string Name)
    {
        if (m_Classes.contains(Name))
        {
            return m_Classes.at(Name);
        }

        GenesisOutputClass* outputClass = new GenesisOutputClass();

        m_Classes.emplace(Name, outputClass);
        return outputClass;
    }

    std::vector<std::string> GenesisOutputData::GetAllAvailableClassNames()
    {
        std::vector<std::string> result = {};

        for (auto currentIterator : m_Classes)
        {
            result.push_back(currentIterator.first);
        }

        return result;
    }

} // namespace genesis::output