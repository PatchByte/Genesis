#ifndef _GENESISPINTRACKER_HPP
#define _GENESISPINTRACKER_HPP

#include <cstdint>
namespace genesis::utils
{

    enum class GenesisPinType
    {
        INPUT = 0,
        OUTPUT = 1
    };

    union GenesisPinValue
    {
    public:
        GenesisPinValue()
        {
            m_PinRawValue = 0;
        }

        static GenesisPinValue Unpack(uintptr_t PinRawValue)
        {
            GenesisPinValue v;
            v.m_PinRawValue = PinRawValue;
            return v;
        }

        static GenesisPinValue Build(unsigned int NodeParentId, unsigned int NodePinId, GenesisPinType Type)
        {
            GenesisPinValue v;

            v.m_NodeParentId = NodeParentId;
            v.m_NodePinId = NodePinId;
            v.m_NodePinType = Type;
        
            return v;
        }

        inline uintptr_t Get()
        {
            return m_PinRawValue;
        }
    public:
        struct {
            unsigned int m_NodeParentId : 32;
            unsigned int m_NodePinId : 29;
            GenesisPinType m_NodePinType : 2;
        };

        uintptr_t m_PinRawValue;
    };

}

#endif // !_GENESISPINTRACKER_HPP