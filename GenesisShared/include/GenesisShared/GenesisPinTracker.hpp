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
        GenesisPinValue(uintptr_t PinRawValue)
        {
            m_PinRawValue = PinRawValue;
        }

        GenesisPinValue(unsigned int NodeParentId, unsigned int NodePinId, GenesisPinType Type)
        {
            m_NodeParentId = NodeParentId;
            m_NodePinId = NodePinId;
            m_NodePinType = Type;
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