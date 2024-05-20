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
        inline GenesisOutputClassVirtualFunction(std::string Name, unsigned int VirtualTableOffset) : m_Name(Name), m_VirtualTableOffset(VirtualTableOffset)
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

        inline unsigned int GetVirtualTableOffset()
        {
            return m_VirtualTableOffset;
        }

    private:
        std::string m_Name;
        unsigned int m_VirtualTableOffset;
    };

    class GenesisOutputClassNonVirtualFunction
    {
    public:
        inline GenesisOutputClassNonVirtualFunction(std::string Name, unsigned long long Offset) : m_Name(Name), m_Offset(Offset)
        {
        }

        inline std::string GetName()
        {
            return m_Name;
        }

        inline unsigned long long GetOffset()
        {
            return m_Offset;
        }

    private:
        std::string m_Name;
        unsigned long long m_Offset;
    };

    class GenesisOutputStaticFunction
    {
    public:
        inline GenesisOutputStaticFunction(std::string Name, unsigned long long Offset) : m_Name(Name), m_Offset(Offset)
        {
        }

        inline std::string GetName()
        {
            return m_Name;
        }

        inline unsigned long long GetOffset()
        {
            return m_Offset;
        }

    private:
        std::string m_Name;
        unsigned long long m_Offset;
    };

    class GenesisOutputStaticVariable
    {
    public:
        inline GenesisOutputStaticVariable(std::string Name, unsigned long long Offset) : m_Name(Name), m_Offset(Offset)
        {
        }

        inline std::string GetName()
        {
            return m_Name;
        }

        inline unsigned long long GetOffset()
        {
            return m_Offset;
        }

    private:
        std::string m_Name;
        unsigned long long m_Offset;
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

        ash::AshResult AddVirtualFunction(GenesisOutputClassVirtualFunction VirtualFunction);
        ash::AshResult AddNonVirtualFunction(GenesisOutputClassNonVirtualFunction NonVirtualFunction);
        ash::AshResult AddStaticFunction(GenesisOutputStaticFunction StaticFunction);
        ash::AshResult AddStaticVariable(GenesisOutputStaticVariable StaticVariable);
        ash::AshResult AddMember(GenesisOutputClassMember Member);

        inline const std::map<std::string, GenesisOutputClassVirtualFunction>& GetVirtualFunctions()
        {
            return m_VirtualFunctions;
        }

        inline const std::map<std::string, GenesisOutputClassNonVirtualFunction>& GetNonVirtualFunctions()
        {
            return m_NonVirtualFunctions;
        }

        inline const std::map<std::string, GenesisOutputStaticFunction>& GetStaticFunctions()
        {
            return m_StaticFunctions;
        }

        inline const std::map<std::string, GenesisOutputStaticVariable>& GetStaticVariables()
        {
            return m_StaticVariables;
        }

        inline const std::map<std::string, GenesisOutputClassMember>& GetMembers()
        {
            return m_Members;
        }

    private:
        std::map<std::string, GenesisOutputClassVirtualFunction> m_VirtualFunctions;
        std::map<std::string, GenesisOutputClassNonVirtualFunction> m_NonVirtualFunctions;
        std::map<std::string, GenesisOutputStaticFunction> m_StaticFunctions;
        std::map<std::string, GenesisOutputStaticVariable> m_StaticVariables;
        std::map<std::string, GenesisOutputClassMember> m_Members;
    };

    class GenesisOutputData
    {
    public:
        GenesisOutputData();
        ~GenesisOutputData();

        GenesisOutputClass* GetOrCreateClass(std::string Name);
        std::vector<std::string> GetAllAvailableClassNames();

        inline GenesisOutputClass* GetGlobalClass()
        {
            return &m_GlobalClass;
        }

    private:
        std::map<std::string, GenesisOutputClass*> m_Classes;
        GenesisOutputClass m_GlobalClass;
    };

} // namespace genesis::output

#endif // !_GENESISOUTPUT_HPP