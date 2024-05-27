#ifndef _GENESISLIVEPACKETS_HPP
#define _GENESISLIVEPACKETS_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshStreamableObject.h"

namespace genesis::live
{

    enum class GenesisLiveConnectionPacketType : unsigned int
    {
        INVALID = 0,
        REQUEST_GENESIS_SERIALIZED_FILE = 1,
        RESPONSE_GENESIS_SERIALIZED_FILE = 2,
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