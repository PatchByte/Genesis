#ifndef _GENESISOUTPUT_HPP
#define _GENESISOUTPUT_HPP

#include "Ash/AshResult.h"
#include <map>
#include <string>
#include <vector>

namespace genesis::output
{

    class GenesisOutputClassVirtualFunction
    {
    public:
        inline GenesisOutputClassVirtualFunction(std::string Name, std::string FunctionDeclaration, unsigned int VirtualTableOffset)
            : m_Name(Name), m_FunctionDeclaration(FunctionDeclaration), m_VirtualTableOffset(VirtualTableOffset)
        {
        }

        // This only checks if (m_VirtualTableOffset % 8) == 0
        inline bool IsValidOffset()
        {
            return (m_VirtualTableOffset % 8) == 0;
        }

        inline std::string GetName()
        {
            return m_Name;
        }

        inline std::string GetFunctionDeclaration()
        {
            return m_FunctionDeclaration;
        }

        inline unsigned int GetVirtualTableOffset()
        {
            return m_VirtualTableOffset;
        }

    private:
        std::string m_Name;
        std::string m_FunctionDeclaration;
        unsigned int m_VirtualTableOffset;
    };

    class GenesisOutputClassMember
    {
    public:
        GenesisOutputClassMember(std::string Name, std::string Type, unsigned int Offset) : m_Name(Name), m_Type(Type), m_Offset(Offset)
        {
        }

        inline std::string GetName()
        {
            return m_Name;
        }

        inline std::string GetType()
        {
            return m_Type;
        }

        inline unsigned int GetOffset()
        {
            return m_Offset;
        }

    private:
        std::string m_Name;
        std::string m_Type;
        unsigned int m_Offset;
    };

    class GenesisOutputClass
    {
    public:
        GenesisOutputClass() = default;
        ~GenesisOutputClass() = default;

        ash::AshResult AddVirtualFunctions(GenesisOutputClassVirtualFunction VirtualFunction);
        ash::AshResult AddMember(GenesisOutputClassMember Member);

        inline const std::map<std::string, GenesisOutputClassVirtualFunction>& GetVirtualFunctions()
        {
            return m_VirtualFunctions;
        }

        inline const std::map<std::string, GenesisOutputClassMember>& GetMembers()
        {
            return m_Members;
        }

    private:
        std::map<std::string, GenesisOutputClassVirtualFunction> m_VirtualFunctions;
        std::map<std::string, GenesisOutputClassMember> m_Members;
    };

    class GenesisOutputData
    {
    public:
        GenesisOutputData();
        ~GenesisOutputData();

        GenesisOutputClass* GetOrCreateClass(std::string Name);
        std::vector<std::string> GetAllAvailableClassNames();

    private:
        std::map<std::string, GenesisOutputClass*> m_Classes;
    };

} // namespace genesis::output

#endif // !_GENESISOUTPUT_HPP