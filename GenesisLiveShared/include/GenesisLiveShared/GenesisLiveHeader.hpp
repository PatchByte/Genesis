#ifndef _GENESISLIVEHEADER_HPP
#define _GENESISLIVEHEADER_HPP

#include "Ash/AshCRC32.h"
#include "Ash/AshStreamableObject.h"
#include "GenesisLiveShared/GenesisLive.hpp"

namespace genesis::live
{

    class GenesisLivePacketHeader : public ash::AshStreamableObject
    {
    public:
        GenesisLivePacketHeader(std::string Identification);
        GenesisLivePacketHeader(std::string Identification, unsigned int Type);
        ~GenesisLivePacketHeader() = default;

        void SetIdentification(std::string Identification);
        inline ash::AshCRC32Value GetIdentification() { return m_Identification; }
        bool IsIdentification(std::string Identification);

        template <typename T>
        inline T GetTypeAs()
        {
            return static_cast<T>(m_Type);
        }

        template<typename T>
        inline void SetType(T Type)
        {
            m_Type = static_cast<unsigned int>(Type);
        }

        inline GenesisPeerId GetSender() { return m_Sender; }
        inline GenesisPeerId GetReceiver() { return m_Receiver; }

        bool Reset();
        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);
    protected:
        ash::AshCRC32Value m_Identification;
        unsigned int m_Type;
        bool m_HasErrorOccurred;
        
        GenesisPeerId m_Sender;
        GenesisPeerId m_Receiver;
    };

} // namespace genesis::live

#endif // !_GENESISLIVEHEADER_HPP