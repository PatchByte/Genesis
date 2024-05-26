#include "GenesisLiveShared/GenesisLiveHeader.hpp"
#include "Ash/AshCRC32.h"

namespace genesis::live
{

    GenesisLivePacketHeader::GenesisLivePacketHeader(std::string Identification): m_Type(-1)
    {
        SetIdentification(Identification);
    }

    GenesisLivePacketHeader::GenesisLivePacketHeader(std::string Identification, unsigned int Type) : m_Type(Type)
    {
        SetIdentification(Identification);
    }

    void GenesisLivePacketHeader::SetIdentification(std::string Identification)
    {
        ash::AshCRC32 identificationCrc = ash::AshCRC32();

        identificationCrc.UpdateWithTemplateValue('hdr ');
        identificationCrc.Update(Identification.data(), Identification.size());

        m_Identification = identificationCrc.GetValue();
    }

    bool GenesisLivePacketHeader::IsIdentification(std::string Identification)
    {
        ash::AshCRC32 identificationCrc = ash::AshCRC32();

        identificationCrc.UpdateWithTemplateValue('hdr ');
        identificationCrc.Update(Identification.data(), Identification.size());

        return m_Identification == identificationCrc.GetValue();
    }

    bool GenesisLivePacketHeader::Reset()
    {
        m_HasErrorOccurred = false;
        m_Type = 0;

        return true;
    }

    bool GenesisLivePacketHeader::Import(ash::AshStream* Stream)
    {
        m_Identification = Stream->Read<ash::AshCRC32Value>();
        m_Type = Stream->Read<unsigned int>();

        m_HasErrorOccurred = Stream->HasErrorOccurred();

        return m_HasErrorOccurred == false;
    }

    bool GenesisLivePacketHeader::Export(ash::AshStream* Stream)
    {
        Stream->Write<ash::AshCRC32Value>(m_Identification);
        Stream->Write<unsigned int>(m_Type);

        m_HasErrorOccurred = Stream->HasErrorOccurred();

        return m_HasErrorOccurred == false;
    }

} // namespace genesis::live