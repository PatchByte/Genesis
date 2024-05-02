#ifndef _GENESISWINDOWS_HPP
#define _GENESISWINDOWS_HPP

#define WINDOWS_IMAGE_SCN_MEM_EXECUTE            0x20000000
#define WINDOWS_IMAGE_SCN_MEM_READ               0x40000000
#define WINDOWS_IMAGE_SCN_MEM_WRITE              0x80000000

namespace windows
{
    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned int DWORD;
    typedef DWORD LONG;
    typedef unsigned long long QWORD;

    typedef unsigned char UCHAR;
    typedef unsigned short USHORT;
    typedef DWORD ULONG;
    typedef unsigned long long ULONGLONG;

    typedef struct _IMAGE_DOS_HEADER
    {
        WORD e_magic;
        WORD e_cblp;
        WORD e_cp;
        WORD e_crlc;
        WORD e_cparhdr;
        WORD e_minalloc;
        WORD e_maxalloc;
        WORD e_ss;
        WORD e_sp;
        WORD e_csum;
        WORD e_ip;
        WORD e_cs;
        WORD e_lfarlc;
        WORD e_ovno;
        WORD e_res[4];
        WORD e_oemid;
        WORD e_oeminfo;
        WORD e_res2[10];
        LONG e_lfanew;
    } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

    typedef struct _IMAGE_FILE_HEADER
    {
        USHORT Machine;
        USHORT NumberOfSections;
        ULONG TimeDateStamp;
        ULONG PointerToSymbolTable;
        ULONG NumberOfSymbols;
        USHORT SizeOfOptionalHeader;
        USHORT Characteristics;
    } IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER; 

    typedef struct _IMAGE_DATA_DIRECTORY
    {
        ULONG VirtualAddress;
        ULONG Size;
    } IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

    typedef struct _IMAGE_OPTIONAL_HEADER64
    {
        USHORT Magic;
        UCHAR MajorLinkerVersion;
        UCHAR MinorLinkerVersion;
        ULONG SizeOfCode;
        ULONG SizeOfInitializedData;
        ULONG SizeOfUninitializedData;
        ULONG AddressOfEntryPoint;
        ULONG BaseOfCode;
        ULONGLONG ImageBase;
        ULONG SectionAlignment;
        ULONG FileAlignment;
        USHORT MajorOperatingSystemVersion;
        USHORT MinorOperatingSystemVersion;
        USHORT MajorImageVersion;
        USHORT MinorImageVersion;
        USHORT MajorSubsystemVersion;
        USHORT MinorSubsystemVersion;
        ULONG Win32VersionValue;
        ULONG SizeOfImage;
        ULONG SizeOfHeaders;
        ULONG CheckSum;
        USHORT Subsystem;
        USHORT DllCharacteristics;
        ULONGLONG SizeOfStackReserve;
        ULONGLONG SizeOfStackCommit;
        ULONGLONG SizeOfHeapReserve;
        ULONGLONG SizeOfHeapCommit;
        ULONG LoaderFlags;
        ULONG NumberOfRvaAndSizes;
        IMAGE_DATA_DIRECTORY DataDirectory[16];
    } IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

    typedef struct _IMAGE_SECTION_HEADER
    {
        UCHAR Name[8];
        union
        {
            ULONG PhysicalAddress;
            ULONG VirtualSize;
        } Misc;
        ULONG VirtualAddress;
        ULONG SizeOfRawData;
        ULONG PointerToRawData;
        ULONG PointerToRelocations;
        ULONG PointerToLinenumbers;
        USHORT NumberOfRelocations;
        USHORT NumberOfLinenumbers;
        ULONG Characteristics;
    } IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER; 

    typedef struct _IMAGE_NT_HEADERS64 
    {
        DWORD Signature;
        IMAGE_FILE_HEADER FileHeader;
        IMAGE_OPTIONAL_HEADER64 OptionalHeader;
    } IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

    typedef struct _IMAGE_BASE_RELOCATION 
    {
        DWORD VirtualAddress;
        DWORD SizeOfBlock;
    } IMAGE_BASE_RELOCATION, *PIMAGE_BASE_RELOCATION;

}

#endif // !_GENESISWINDOWS_HPP