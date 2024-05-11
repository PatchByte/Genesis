#include "GenesisShared/GenesisOutput.hpp"

namespace genesis::output
{

    ash::AshResult GenesisOutputClass::AddVirtualFunctions(GenesisOutputClassVirtualFunction VirtualFunction)
    {
        if (m_VirtualFunctions.contains(VirtualFunction.GetName()))
        {
            return ash::AshResult(false);
        }

        m_VirtualFunctions.emplace(VirtualFunction.GetName(), VirtualFunction);

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

} // namespace genesis::output