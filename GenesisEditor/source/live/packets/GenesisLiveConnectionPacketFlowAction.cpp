#include "AshObjects/AshString.h"
#include "GenesisEditor/live/GenesisLivePackets.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include <cstdint>

namespace genesis::live
{

    GenesisLiveConnectionPacketFlowAction::GenesisLiveConnectionPacketFlowAction() : m_FlowTargetName(), m_ActionType(ActionType::INVALID)
    {
        m_Action.m_RawValue = 0;
    }

    bool GenesisLiveConnectionPacketFlowAction::Import(ash::AshStream* Stream)
    {
        ash::objects::AshAsciiString flowTargetNameObject = ash::objects::AshAsciiString();

        m_ActionType = static_cast<ActionType>(Stream->Read<unsigned int>());

        if(flowTargetNameObject.Import(Stream) == false)
        {
            return false;
        }

        m_FlowTargetName = flowTargetNameObject.GetText();

        switch (m_ActionType)
        {
        case ActionType::LINK_CREATED:
        {
            m_Action.m_LinkCreated.m_FromLinkId = Stream->Read<uintptr_t>();
            m_Action.m_LinkCreated.m_ToLinkId = Stream->Read<uintptr_t>();
            break;
        }
        case ActionType::LINK_DELETED:
        {
            m_Action.m_LinkDeleted.m_LinkId = Stream->Read<uintptr_t>();
            break;
        }
        case ActionType::SET_POSITION_NODE:
        {
            m_Action.m_SetPositionNode.m_NodeId = Stream->Read<uintptr_t>();
            m_Action.m_SetPositionNode.m_X = Stream->Read<float>();
            m_Action.m_SetPositionNode.m_Y = Stream->Read<float>();
            break;
        }
        case ActionType::NODE_CREATED:
        {
            m_Action.m_NodeCreated.m_OperationType = static_cast<operations::GenesisOperationType>(Stream->Read<unsigned int>());
            m_Action.m_NodeCreated.m_ExpectedNodeIdResult = Stream->Read<uintptr_t>();
            m_Action.m_NodeCreated.m_X = Stream->Read<float>();
            m_Action.m_NodeCreated.m_Y = Stream->Read<float>();
            break;
        }
        case ActionType::NODE_DELETED:
        {
            m_Action.m_NodeDeleted.m_NodeId = Stream->Read<uintptr_t>();
            break;
        }
        default:
            printf("Cry and shitting yourself %i\n", __LINE__);
            exit(-1);
            break;
        }

        return Stream->IsOkay();
    }

    bool GenesisLiveConnectionPacketFlowAction::Export(ash::AshStream* Stream)
    {
        Stream->Write(static_cast<unsigned int>(m_ActionType));
        ash::objects::AshAsciiString(m_FlowTargetName).Export(Stream);

        switch (m_ActionType)
        {
        case ActionType::LINK_CREATED:
        {
            Stream->Write<uintptr_t>(m_Action.m_LinkCreated.m_FromLinkId);
            Stream->Write<uintptr_t>(m_Action.m_LinkCreated.m_ToLinkId);
            break;
        }
        case ActionType::LINK_DELETED:
        {
            Stream->Write<uintptr_t>(m_Action.m_LinkDeleted.m_LinkId);
            break;
        }
        case ActionType::SET_POSITION_NODE:
        {
            Stream->Write<uintptr_t>(m_Action.m_SetPositionNode.m_NodeId);
            Stream->Write<float>(m_Action.m_SetPositionNode.m_X);
            Stream->Write<float>(m_Action.m_SetPositionNode.m_Y);
            break;
        }
        case ActionType::NODE_CREATED:
        {
            Stream->Write<unsigned int>(static_cast<unsigned int>(m_Action.m_NodeCreated.m_OperationType));
            Stream->Write<uintptr_t>(m_Action.m_NodeCreated.m_ExpectedNodeIdResult);
            Stream->Write<float>(m_Action.m_NodeCreated.m_X);
            Stream->Write<float>(m_Action.m_NodeCreated.m_Y);
            break;
        }
        case ActionType::NODE_DELETED:
        {
            Stream->Write<uintptr_t>(m_Action.m_NodeDeleted.m_NodeId);
            break;
        }
        default:
            printf("Cry and shitting yourself %i\n", __LINE__);
            exit(-1);
            break;
        }

        return Stream->IsOkay();
    }

} // namespace genesis::live