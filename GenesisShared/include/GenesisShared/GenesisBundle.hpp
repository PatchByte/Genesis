#ifndef _GENESISBUNDLE_HPP
#define _GENESISBUNDLE_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshStreamableObject.h"
#include "GenesisShared/GenesisFlow.hpp"
#include <functional>
#include <map>
namespace genesis
{

    class GenesisBundle : public ash::AshStreamableObject
    {
    public:
        using sdFlowFactory = std::function<GenesisFlow*(void* Reserved)>;

        GenesisBundle(sdFlowFactory FlowFactory);
        ~GenesisBundle();

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

        // Factory Function

        static GenesisFlow* sfDefaultFactory();
    protected:
        sdFlowFactory m_FlowFactory;
        std::map<std::string, GenesisFlow*> m_Flows;
    };

}

#endif // !_GENESISBUNDLE_HPP