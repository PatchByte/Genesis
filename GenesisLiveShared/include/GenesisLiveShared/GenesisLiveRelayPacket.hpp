#ifndef _GENESISLIVERELAYPACKET_HPP
#define _GENESISLIVERELAYPACKET_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshCRC32.h"
#include "Ash/AshResult.h"
#include "Ash/AshStreamableObject.h"
#include "GenesisLiveShared/GenesisLive.hpp"

namespace genesis::live
{

    enum class GenesisLiveRelayPacketType : unsigned int
    {
        INVALID = 0,
        CLIENT_CONNECT_REQUEST = 1,
        CLIENT_CONNECT_RESPONSE = 2,
        PING = 3,
        FORWARD_TO = 4,
        FORWARD_FROM = 5,
        CLIENT_LEFT = 6
    };

    class GenesisLiveRelayPacketBase : public ash::AshStreamableObject
    {
    public:
        inline GenesisLiveRelayPacketBase() : m_HasAnyErrorOccurred(false)
        {
        }

        virtual ~GenesisLiveRelayPacketBase() = default;

        virtual GenesisLiveRelayPacketType GetType()
        {
            return GenesisLiveRelayPacketType::INVALID;
        }

    protected:
        bool m_HasAnyErrorOccurred;
    };

    class GenesisLiveRelayPacketClientConnectRequest : public GenesisLiveRelayPacketBase
    {
    public:
        GenesisLiveRelayPacketClientConnectRequest();
        ~GenesisLiveRelayPacketClientConnectRequest() = default;

        GenesisLiveRelayPacketType GetType()
        {
            return GenesisLiveRelayPacketType::CLIENT_CONNECT_REQUEST;
        }

        inline std::string GetClientName()
        {
            return m_ClientName;
        }

        inline void SetClientName(std::string ClientName)
        {
            m_ClientName = ClientName;
        }

        inline std::string GetKey()
        {
            return m_ClientKey;
        }

        inline void SetKey(std::string ClientKey)
        {
            m_ClientKey = ClientKey;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_ClientName;
        std::string m_ClientKey;
    };

    class GenesisLiveRelayPacketClientConnectResponse : public GenesisLiveRelayPacketBase
    {
    public:
        // This packet can also be sent to other clients which also have connected to signal a new peer has been connected.
        enum class ReasonType : unsigned int
        {
            INVALID = 0,
            FORCE_ASSIGN_TO_YOU = 1,
            CLIENT_JOINED = 2
        };

        GenesisLiveRelayPacketClientConnectResponse();
        ~GenesisLiveRelayPacketClientConnectResponse() = default;

        GenesisLiveRelayPacketType GetType()
        {
            return GenesisLiveRelayPacketType::CLIENT_CONNECT_RESPONSE;
        }

        inline std::string GetAssignedClientName()
        {
            return m_AssignedClientName;
        }

        inline void SetAssignedClientName(std::string AssignedClientName)
        {
            m_AssignedClientName = AssignedClientName;
        }

        inline GenesisPeerId GetAssignedPeerId()
        {
            return m_AssignedPeerId;
        }

        inline void SetAssignedPeerId(GenesisPeerId AssignedPeerId)
        {
            m_AssignedPeerId = AssignedPeerId;
        }

        inline ReasonType GetReason()
        {
            return m_Reason;
        }

        inline void SetReason(ReasonType Reason)
        {
            m_Reason = Reason;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_AssignedClientName;
        GenesisPeerId m_AssignedPeerId;
        ReasonType m_Reason;
    };

    class GenesisLiveRelayPacketPing : public GenesisLiveRelayPacketBase
    {
    public:
        GenesisLiveRelayPacketPing();
        ~GenesisLiveRelayPacketPing() = default;

        GenesisLiveRelayPacketType GetType()
        {
            return GenesisLiveRelayPacketType::PING;
        }

        void SetChallenge(ash::AshCRC32Value Challenge);
        bool IsAnswerCorrect();

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);
    protected:
        ash::AshCRC32Value m_Challenge;
        ash::AshCRC32Value m_Answer;
    };

    // GenesisLiveRelayPacketUtils

    class GenesisLiveRelayPacketUtils
    {
    public:
        static GenesisLiveRelayPacketBase* sfCreateRelayPacketByType(GenesisLiveRelayPacketType Type);
        static ash::AshCustomResult<ash::AshBuffer*> sfSerializePacket(std::string Identification, GenesisLiveRelayPacketBase* Packet);
        static ash::AshCustomResult<GenesisLiveRelayPacketBase*> sfDeserializePacket(std::string Identification, ash::AshBuffer* Buffer);
    };

} // namespace genesis::live

#endif // !_GENESISLIVERELAYPACKET_HPP