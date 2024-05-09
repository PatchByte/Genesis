#include "GenesisShared/GenesisLoadedFile.hpp"

namespace genesis::common
{
    GenesisLoadedFile::GenesisLoadedFile(std::filesystem::path TargetPath):
        m_TargetPath(TargetPath),
        m_TargetBuffer()
    {}

    GenesisLoadedFile::~GenesisLoadedFile()
    {
        m_TargetBuffer.ReleaseMemory();
    }

    ash::AshResult GenesisLoadedFile::Load()
    {
        if(std::filesystem::exists(m_TargetPath) == false)
        {
            return ash::AshResult(false, "");
        }

        if(auto res = m_TargetBuffer.ReadFromFile(m_TargetPath); res.HasError())
        {
            return res;
        }

        return LoadBuffer();
    }

    ash::AshResult GenesisLoadedFile::LoadBuffer()
    {
        if(m_TargetBuffer.IsAllocated() == false)
        {
            return ash::AshResult(false, "Buffer is not allocated.");
        }

        m_DosHeader = m_TargetBuffer.GetBufferAtOffset<windows::IMAGE_DOS_HEADER>(0x00);

        if(m_DosHeader->e_magic != 0x5a4d)
        {
            return ash::AshResult(false, "e_magic is wrong.");
        }

        m_NtHeaders = m_TargetBuffer.GetBufferAtOffset<windows::IMAGE_NT_HEADERS64>(m_DosHeader->e_lfanew);
        
        if(m_NtHeaders == nullptr)
        {
            return ash::AshResult(false, "Couldn't get the nt header!");
        }

        m_FileHeader = &m_NtHeaders->FileHeader;
        m_OptionalHeader = &m_NtHeaders->OptionalHeader;

        m_FirstSectionHeader = reinterpret_cast<windows::IMAGE_SECTION_HEADER*>(
            reinterpret_cast<unsigned long long>(m_OptionalHeader) + m_FileHeader->SizeOfOptionalHeader
        );

        return ash::AshResult(true);
    }

    ash::AshResult GenesisLoadedFile::Save(std::filesystem::path Path)
    {
        if(std::filesystem::exists(Path.root_directory()) == false)
        {
            return ash::AshResult(false, "Paths root directory is not existing.");
        }

        m_TargetPath = Path;
        return m_TargetBuffer.WriteToFile(Path);
    }

    Address GenesisLoadedFile::TranslateFileOffsetToVirtualAddress(Address FileOffset)
    {
        for(unsigned int currentSectionIndex = 0; currentSectionIndex < m_NtHeaders->FileHeader.NumberOfSections; currentSectionIndex++)
        {
            windows::IMAGE_SECTION_HEADER* currentSectionHeader = m_FirstSectionHeader + currentSectionIndex;

            if((currentSectionHeader->PointerToRawData) <= FileOffset && (currentSectionHeader->PointerToRawData + currentSectionHeader->SizeOfRawData) >= FileOffset)
            {
                return (currentSectionHeader->VirtualAddress) + (FileOffset- currentSectionHeader->PointerToRawData) + (m_OptionalHeader->ImageBase);
            }
        }

        return -1;
    }

    Address GenesisLoadedFile::TranslateBufferAddressToVirtualAddress(Address BufferAddress)
    {
        Address startAddress = reinterpret_cast<Address>(m_TargetBuffer.GetPointer());
        Address endAddress = startAddress + m_TargetBuffer.GetSize();

        if(BufferAddress >= startAddress && BufferAddress <= endAddress)
        {
            return TranslateFileOffsetToVirtualAddress(BufferAddress - startAddress);
        }

        return -1;
    }

    Address GenesisLoadedFile::TranslateVirtualAddressToFileOffset(Address VirtualAddress)
    {
        for(unsigned int currentSectionIndex = 0; currentSectionIndex < m_NtHeaders->FileHeader.NumberOfSections; currentSectionIndex++)
        {
            windows::IMAGE_SECTION_HEADER* currentSectionHeader = m_FirstSectionHeader + currentSectionIndex;
            Address currentSectionVirtualAddress = currentSectionHeader->VirtualAddress + m_OptionalHeader->ImageBase;

            if(currentSectionVirtualAddress <= VirtualAddress && (currentSectionVirtualAddress + currentSectionHeader->SizeOfRawData) >= VirtualAddress)
            {
                return VirtualAddress - currentSectionVirtualAddress + currentSectionHeader->PointerToRawData;
            }
        }

        return -1;
    }

    Address GenesisLoadedFile::TranslateVirtualAddressToImageOffset(Address VirtualAddress)
    {
        return VirtualAddress - m_OptionalHeader->ImageBase;
    }

    windows::IMAGE_SECTION_HEADER* GenesisLoadedFile::GetSectionInWhichFileOffsetResides(Address FileOffset)
    {
        for(unsigned int currentSectionIndex = 0; currentSectionIndex < m_NtHeaders->FileHeader.NumberOfSections; currentSectionIndex++)
        {
            windows::IMAGE_SECTION_HEADER* currentSectionHeader = m_FirstSectionHeader + currentSectionIndex;

            if((currentSectionHeader->PointerToRawData) <= FileOffset && (currentSectionHeader->PointerToRawData + currentSectionHeader->SizeOfRawData) >= FileOffset)
            {
                return currentSectionHeader;
            }
        }

        return nullptr;
    }

    windows::IMAGE_SECTION_HEADER* GenesisLoadedFile::GetSectionInWhichVirtualAddressResides(Address VirtualAddress)
    {
        for(unsigned int currentSectionIndex = 0; currentSectionIndex < m_NtHeaders->FileHeader.NumberOfSections; currentSectionIndex++)
        {
            windows::IMAGE_SECTION_HEADER* currentSectionHeader = m_FirstSectionHeader + currentSectionIndex;
            Address currentSectionVirtualAddress = currentSectionHeader->VirtualAddress + m_OptionalHeader->ImageBase;

            if(currentSectionVirtualAddress <= VirtualAddress && (currentSectionVirtualAddress + currentSectionHeader->SizeOfRawData) >= VirtualAddress)
            {
                return currentSectionHeader;
            }
        }

        return nullptr;
    }

    SectionArray GenesisLoadedFile::GetAllExecutableSections()
    {
        SectionArray result = {};

        for(unsigned int currentSectionIndex = 0; currentSectionIndex < m_NtHeaders->FileHeader.NumberOfSections; currentSectionIndex++)
        {
            windows::IMAGE_SECTION_HEADER* currentSectionHeader = m_FirstSectionHeader + currentSectionIndex;

            if(currentSectionHeader->Characteristics & WINDOWS_IMAGE_SCN_MEM_EXECUTE)
            {
                result.push_back(currentSectionHeader);
            }
        }

        return result;
    }

    std::string GenesisLoadedFile::GetFilenameWithExtension()
    {
        return m_TargetPath.filename().generic_string();
    }
}