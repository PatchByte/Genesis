#ifndef _GENESISBUNDLE_HPP
#define _GENESISBUNDLE_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshStreamableObject.h"
namespace genesis
{



    class GenesisBundle : public ash::AshStreamableObject
    {
    public:
        
    protected:
        ash::AshBuffer m_ReservedBuffer;
    };

}

#endif // !_GENESISBUNDLE_HPP