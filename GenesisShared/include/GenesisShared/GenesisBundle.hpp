#ifndef _GENESISBUNDLE_HPP
#define _GENESISBUNDLE_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
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

        GenesisBundle(sdFlowFactory FlowFactory = sfDefaultFactory);
        ~GenesisBundle();

        ash::AshResult AddFlow(std::string FlowName, GenesisFlow* Flow);
        ash::AshResult RemoveFlow(std::string FlowName, bool FreeFlow = true);
        inline std::map<std::string, GenesisFlow*> GetFlows() { return m_Flows; }

        virtual void Reset();
        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

        // Factory Function

        static GenesisFlow* sfDefaultFactory(void* Reserved);
    protected:
        sdFlowFactory m_FlowFactory;
        std::map<std::string, GenesisFlow*> m_Flows;
        void* m_ReservedFactoryValue;
    };

}

#endif // !_GENESISBUNDLE_HPP