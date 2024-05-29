#include "AshObjects/AshString.h"
#include "GenesisEditor/live/GenesisLivePackets.hpp"

namespace genesis::live
{

    GenesisLiveConnectionPacketBundleAction::GenesisLiveConnectionPacketBundleAction():
        m_ActionType(ActionType::INVALID),
        m_ExtensibleName1(),
        m_ExtensibleName2()
    {}

    GenesisLiveConnectionPacketBundleAction::GenesisLiveConnectionPacketBundleAction(ActionType Action, std::string ExtensibleName1, std::string ExtensibleName2):
        m_ActionType(Action),
        m_ExtensibleName1(ExtensibleName1),
        m_ExtensibleName2(ExtensibleName2)
    {}

    bool GenesisLiveConnectionPacketBundleAction::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString extensibleNameObject1 = ash::objects::AshAsciiString(), extensibleNameObject2 = ash::objects::AshAsciiString(); 

        m_ActionType = static_cast<ActionType>(Stream->Read<unsigned int>());

        if(extensibleNameObject1.Import(Stream) == false || extensibleNameObject2.Import(Stream) == false)
        {
            return false;
        }

        m_ExtensibleName1 = extensibleNameObject1.GetText();
        m_ExtensibleName2 = extensibleNameObject2.GetText();

        return Stream->IsOkay();
    }

    bool GenesisLiveConnectionPacketBundleAction::Export(ash::AshStream* Stream)
    {
        Stream->Write(static_cast<unsigned int>(m_ActionType));

        ash::objects::AshAsciiString(m_ExtensibleName1).Export(Stream);
        ash::objects::AshAsciiString(m_ExtensibleName2).Export(Stream);

        return Stream->IsOkay();
    }

}