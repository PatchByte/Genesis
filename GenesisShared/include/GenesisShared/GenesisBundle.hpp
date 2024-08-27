#ifndef _GENESISBUNDLE_HPP
#define _GENESISBUNDLE_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshResult.h"
#include "Ash/AshStreamableObject.h"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include <functional>
#include <map>

namespace genesis
{

    class GenesisBundle : public ash::AshStreamableObject
    {
    public:
        using sdFlowFactory = std::function<GenesisFlow*(void* Reserved)>;

        static GenesisFlow* sfDefaultFlowFactory(void*);

        GenesisBundle(sdFlowFactory FlowFactory = sfDefaultFlowFactory);
        ~GenesisBundle();

        ash::AshResult CreateFlow(std::string FlowName);
        ash::AshResult AddFlow(std::string FlowName, GenesisFlow* Flow); //! @warning Consumes `Flow`
        ash::AshResult RemoveFlow(std::string FlowName, bool FreeFlow = true);

        inline bool HasFlow(std::string FlowName)
        {
            return m_Flows.contains(FlowName);
        }

        inline GenesisFlow* GetFlow(std::string FlowName)
        {
            return HasFlow(FlowName) ? m_Flows.at(FlowName) : nullptr;
        }

        inline std::map<std::string, GenesisFlow*> GetFlows()
        {
            return m_Flows;
        }

        ash::AshResult ProcessBundle(output::GenesisOutputData* OutputData, common::GenesisLoadedFile* LoadedFile);

        virtual void Reset();
        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        sdFlowFactory m_FlowFactory;
        std::map<std::string, GenesisFlow*> m_Flows;
        void* m_ReservedFactoryValue;
    };

} // namespace genesis

#endif // !_GENESISBUNDLE_HPP
