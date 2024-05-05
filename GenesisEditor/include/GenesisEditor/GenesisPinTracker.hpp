#ifndef _GENESISPINTRACKER_HPP
#define _GENESISPINTRACKER_HPP

namespace genesis::utils
{

    union GenesisPinValue
    {
    public:
        struct {
            unsigned short m_NodeParentId;
            unsigned short m_NodePinId;
        };
        unsigned int m_PinRawValue;
    };

    class GenesisPinTracker
    {
    public:
    private:
    };

}

#endif // !_GENESISPINTRACKER_HPP