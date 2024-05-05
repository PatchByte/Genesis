#ifndef _GENESISPINTRACKER_HPP
#define _GENESISPINTRACKER_HPP

namespace genesis::utils
{

    union GenesisPinValue
    {
    public:
        GenesisPinValue(unsigned short NodeParentId, unsigned short NodePinId)
        {
            m_NodeParentId = NodeParentId;
            m_NodePinId = NodePinId;
        }

        inline GenesisPinValue& Make(unsigned short NodeParentId, unsigned short NodePinId)
        {
            m_NodeParentId = NodeParentId;
            m_NodePinId = NodePinId;

            return *this;
        }

        inline unsigned int Get()
        {
            return m_PinRawValue;
        }
    public:
        struct {
            unsigned short m_NodeParentId;
            unsigned short m_NodePinId;
        };
        unsigned int m_PinRawValue;
    };

}

#endif // !_GENESISPINTRACKER_HPP