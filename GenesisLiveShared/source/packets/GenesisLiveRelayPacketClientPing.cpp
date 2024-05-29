#include "Ash/AshCRC32.h"
#include "AshObjects/AshString.h"
#include "GenesisLiveShared/GenesisLive.hpp"
#include "GenesisLiveShared/GenesisLiveRelayPacket.hpp"

namespace genesis::live
{

    GenesisLiveRelayPacketPing::GenesisLiveRelayPacketPing() : m_Challenge(), m_Answer()
    {
    }

    void GenesisLiveRelayPacketPing::SetChallenge(ash::AshCRC32Value Challenge)
    {
        ash::AshCRC32 crc = ash::AshCRC32();
        crc.UpdateWithTemplateValue(m_Challenge);
        
        m_Challenge = Challenge;
        m_Answer = crc.GetValue();
    }
    
    bool GenesisLiveRelayPacketPing::IsAnswerCorrect()
    {
        ash::AshCRC32 crc = ash::AshCRC32();
        crc.UpdateWithTemplateValue(m_Challenge);

        return m_Answer == crc.GetValue();
    }

    bool GenesisLiveRelayPacketPing::Import(ash::AshStream* Stream)
    {
        m_Challenge = Stream->Read<ash::AshCRC32Value>();
        m_Answer = Stream->Read<ash::AshCRC32Value>();

        return Stream->IsOkay();
    }

    bool GenesisLiveRelayPacketPing::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshCRC32Value>(m_Challenge);
        Stream->Write<ash::AshCRC32Value>(m_Answer);

        return Stream->IsOkay();
    }

} // namespace genesis::live