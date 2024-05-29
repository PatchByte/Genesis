#ifndef _GENESISLIVEPACKETS_HPP
#define _GENESISLIVEPACKETS_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshStreamableObject.h"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::live
{

    enum class GenesisLiveConnectionPacketType : unsigned int
    {
        INVALID = 0,
        REQUEST_GENESIS_SERIALIZED_FILE = 1,
        RESPONSE_GENESIS_SERIALIZED_FILE = 2,
        BUNDLE_ACTION = 3,
        FLOW_ACTION = 4,
    };

    class GenesisLiveConnectionPacketBase : public ash::AshStreamableObject
    {
    public:
        inline GenesisLiveConnectionPacketBase() : m_HasAnyErrorOccurred(false)
        {
        }

        virtual ~GenesisLiveConnectionPacketBase() = default;

        virtual GenesisLiveConnectionPacketType GetType()
        {
            return GenesisLiveConnectionPacketType::INVALID;
        }

    protected:
        bool m_HasAnyErrorOccurred;
    };

    class GenesisLiveConnectionPacketRequestSerializedFile : public GenesisLiveConnectionPacketBase
    {
    public:
        GenesisLiveConnectionPacketRequestSerializedFile() = default;
        ~GenesisLiveConnectionPacketRequestSerializedFile() = default;

        GenesisLiveConnectionPacketType GetType()
        {
            return GenesisLiveConnectionPacketType::REQUEST_GENESIS_SERIALIZED_FILE;
        }

        bool Import(ash::AshStream* Stream)
        {
            return true;
        }

        bool Export(ash::AshStream* Stream)
        {
            return true;
        }
    };

    class GenesisLiveConnectionPacketResponseSerializedFile : public GenesisLiveConnectionPacketBase
    {
    public:
        static constexpr ash::AshSize smBufferMaxSize = 0xFFFFF;

        GenesisLiveConnectionPacketResponseSerializedFile();
        ~GenesisLiveConnectionPacketResponseSerializedFile() = default;

        GenesisLiveConnectionPacketType GetType()
        {
            return GenesisLiveConnectionPacketType::RESPONSE_GENESIS_SERIALIZED_FILE;
        }

        inline ash::AshBuffer& GetSerializedFile()
        {
            return m_SerializedFile;
        }

        inline void SetSerializedFile(ash::AshBuffer SerializedFile)
        {
            m_SerializedFile = SerializedFile;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        ash::AshBuffer m_SerializedFile;
    };

    class GenesisLiveConnectionPacketBundleAction : public GenesisLiveConnectionPacketBase
    {
    public:
        enum class ActionType : unsigned int
        {
            INVALID = 0,
            CREATE_FLOW = 1,
            DELETE_FLOW = 2,
            RENAME_FLOW = 3
        };

        GenesisLiveConnectionPacketBundleAction();
        GenesisLiveConnectionPacketBundleAction(ActionType Action, std::string ExtensibleName1, std::string ExtensibleName2);
        ~GenesisLiveConnectionPacketBundleAction() = default;

        GenesisLiveConnectionPacketType GetType()
        {
            return GenesisLiveConnectionPacketType::BUNDLE_ACTION;
        }

        inline ActionType GetActionType()
        {
            return m_ActionType;
        }

        inline void SetActionType(ActionType Action)
        {
            m_ActionType = Action;
        }

        inline std::string GetExtensibleName1()
        {
            return m_ExtensibleName1;
        }

        inline void SetExtensibleName1(std::string ExtensibleName)
        {
            m_ExtensibleName1 = ExtensibleName;
        }

        inline std::string GetExtensibleName2()
        {
            return m_ExtensibleName2;
        }

        inline void SetExtensibleName2(std::string ExtensibleName)
        {
            m_ExtensibleName2 = ExtensibleName;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        ActionType m_ActionType;

        // Can't use union because class deconstructors etc.

        std::string m_ExtensibleName1;
        std::string m_ExtensibleName2;
    };

    class GenesisLiveConnectionPacketFlowAction : public GenesisLiveConnectionPacketBase
    {
    public:
        enum class ActionType : unsigned int
        {
            INVALID = 0,
            LINK_CREATED = 1,
            LINK_DELETED = 2,
            SET_POSITION_NODE = 3,
            NODE_CREATED = 4,
            NODE_DELETED = 5,
        };

        class ActionLinkCreated
        {
        public:
            uintptr_t m_FromLinkId;
            uintptr_t m_ToLinkId;
        };

        class ActionLinkDeleted
        {
        public:
            uintptr_t m_LinkId;
        };

        class ActionSetPositionNode
        {
        public:
            uintptr_t m_NodeId;
            float m_X;
            float m_Y;
        };

        class ActionNodeCreated
        {
        public:
            operations::GenesisOperationType m_OperationType;
            uintptr_t m_ExpectedNodeIdResult;
            float m_X;
            float m_Y;
        };

        class ActionNodeDeleted
        {
        public:
            uintptr_t m_NodeId;
        };

        GenesisLiveConnectionPacketFlowAction();
        ~GenesisLiveConnectionPacketFlowAction() = default;

        GenesisLiveConnectionPacketType GetType()
        {
            return GenesisLiveConnectionPacketType::FLOW_ACTION;
        }

        inline std::string GetFlowTargetName()
        {
            return m_FlowTargetName;
        }

        inline void SetFlowTargetName(std::string FlowTargetName)
        {
            m_FlowTargetName = FlowTargetName;
        }

        inline ActionType GetActionType()
        {
            return m_ActionType;
        }

        inline void SetActionType(ActionType Action)
        {
            m_ActionType = Action;
        }

        // Action Link Created
        
        inline void SetAsActionLinkCreated(ActionLinkCreated LinkCreated)
        {
            m_ActionType = ActionType::LINK_CREATED;
            m_Action.m_LinkCreated = LinkCreated;
        }

        inline const ActionLinkCreated& GetAsActionLinkCreated()
        {
            return m_Action.m_LinkCreated;
        }

        // Action Link Deleted

        inline const ActionLinkDeleted& GetAsActionLinkDeleted()
        {
            return m_Action.m_LinkDeleted;
        }

        inline void SetAsActionLinkDeleted(ActionLinkDeleted LinkDeleted)
        {
            m_ActionType = ActionType::LINK_DELETED;
            m_Action.m_LinkDeleted = LinkDeleted;
        }

        // Action Set Position Node

        inline const ActionSetPositionNode& GetAsActionSetPositionNode()
        {
            return m_Action.m_SetPositionNode;
        }

        inline void SetAsActionSetPositionNode(ActionSetPositionNode SetNodePositionNode)
        {
            m_ActionType = ActionType::SET_POSITION_NODE;
            m_Action.m_SetPositionNode = SetNodePositionNode;
        }

        // Action Create Node

        inline const ActionNodeCreated& GetAsActionNodeCreated()
        {
            return m_Action.m_NodeCreated;
        } 

        inline void SetAsActionNodeCreated(ActionNodeCreated NodeCreated)
        {
            m_ActionType = ActionType::NODE_CREATED;
            m_Action.m_NodeCreated = NodeCreated;
        }

        // Action Delete Node

        inline const ActionNodeDeleted& GetAsActionNodeDeleted()
        {
            return m_Action.m_NodeDeleted;
        }

        inline void SetAsActionNodeDeleted(ActionNodeDeleted NodeDeleted)
        {
            m_ActionType = ActionType::NODE_DELETED;
            m_Action.m_NodeDeleted = NodeDeleted;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_FlowTargetName;
        ActionType m_ActionType;

        union
        {
            ActionLinkCreated m_LinkCreated;
            ActionLinkDeleted m_LinkDeleted;
            ActionSetPositionNode m_SetPositionNode;
            ActionNodeCreated m_NodeCreated;
            ActionNodeDeleted m_NodeDeleted;
            uintptr_t m_RawValue;
        } m_Action;
    };

    // GenesisLiveRelayPacketUtils

    class GenesisLiveConnectionPacketUtils
    {
    public:
        static GenesisLiveConnectionPacketBase* sfCreateRelayPacketByType(GenesisLiveConnectionPacketType Type);
        static ash::AshCustomResult<ash::AshBuffer*> sfSerializePacket(std::string Identification, GenesisLiveConnectionPacketBase* Packet);
        static ash::AshCustomResult<GenesisLiveConnectionPacketBase*> sfDeserializePacket(std::string Identification, ash::AshBuffer* Buffer);
    };

} // namespace genesis::live

#endif // !_GENESISLIVEPACKETS_HPP