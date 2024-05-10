#ifndef _GENESISOUTPUT_HPP
#define _GENESISOUTPUT_HPP

#include <map>
#include <string>
#include <vector>

namespace genesis::output
{

    class GenesisOutputClassVirtualFunction
    {
    public:
        inline GenesisOutputClassVirtualFunction(std::string Name, std::string ReturnType, unsigned int VirtualTableOffset)
            : m_Name(Name), m_ReturnType(ReturnType), m_VirtualTableOffset(VirtualTableOffset)
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

        inline std::string GetReturnType()
        {
            return m_ReturnType;
        }

        inline unsigned int GetVirtualTableOffset()
        {
            return m_VirtualTableOffset;
        }

    private:
        std::string m_Name;
        std::string m_ReturnType;
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
        GenesisOutputClass();
        ~GenesisOutputClass();
    private:
        std::map<std::string, GenesisOutputClassVirtualFunction*> m_VirtualFunctions;
        std::map<std::string, GenesisOutputClassMember*> m_Members;
    };

} // namespace genesis::output

#endif // !_GENESISOUTPUT_HPP