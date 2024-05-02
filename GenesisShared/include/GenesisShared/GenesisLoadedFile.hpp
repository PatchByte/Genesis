#ifndef _GENESISLOADEDFILE_HPP
#define _GENESISLOADEDFILE_HPP

#include "Ash/AshReferenceableBuffer.h"
#include "Ash/AshResult.h"
#include "GenesisShared/GenesisWindows.hpp"

#include <filesystem>

namespace genesis::common
{

    using Address = unsigned long long;
    using SectionArray = std::vector<windows::IMAGE_SECTION_HEADER*>;

    class GenesisLoadedFile
    {
    public:
        explicit GenesisLoadedFile(std::filesystem::path TargetPath);
        virtual ~GenesisLoadedFile();

        ash::AshResult Load();
        ash::AshResult LoadBuffer();
        ash::AshResult Save(std::filesystem::path Path);

        inline std::filesystem::path GetTargetPath() { return m_TargetPath; }
        inline ash::AshReferenceableBuffer& GetTargetBuffer() { return m_TargetBuffer; }
        inline windows::IMAGE_DOS_HEADER* GetDosHeader() { return m_DosHeader; }
        inline windows::IMAGE_NT_HEADERS64* GetNtHeaders() { return m_NtHeaders; }
        inline windows::IMAGE_FILE_HEADER* GetFileHeader() { return m_FileHeader; }
        inline windows::IMAGE_OPTIONAL_HEADER64* GetOptionalHeader() { return m_OptionalHeader; }
        inline windows::IMAGE_SECTION_HEADER* GetFirstSectionHeader() { return m_FirstSectionHeader; }

        Address TranslateFileOffsetToVirtualAddress(Address FileOffset);
        Address TranslateBufferAddressToVirtualAddress(Address BufferAddress);
        
        Address TranslateVirtualAddressToFileOffset(Address VirtualAddress);
        // Example: Test+0xdeadbeef
        Address TranslateVirtualAddressToImageOffset(Address VirtualAddress);

        windows::IMAGE_SECTION_HEADER* GetSectionInWhichFileOffsetResides(Address FileOffset);
        windows::IMAGE_SECTION_HEADER* GetSectionInWhichVirtualAddressResides(Address VirtualAddress);

        SectionArray GetAllExecutableSections();
        std::string GetFilenameWithExtension();
        std::string GetFilenameWithoutExtension();
    protected:
        std::filesystem::path m_TargetPath;
        ash::AshReferenceableBuffer m_TargetBuffer;

        windows::IMAGE_DOS_HEADER* m_DosHeader;
        windows::IMAGE_NT_HEADERS64* m_NtHeaders;
        windows::IMAGE_FILE_HEADER* m_FileHeader;
        windows::IMAGE_OPTIONAL_HEADER64* m_OptionalHeader;
        windows::IMAGE_SECTION_HEADER* m_FirstSectionHeader;
    };

}

#endif // !_GENESISLOADEDFILE_HPP