#ifndef _GENESISMERGEFLOW_HPP
#define _GENESISMERGEFLOW_HPP

#include "Ash/AshBuffer.h"
#include "Ash/AshCRC32.h"
#include "Ash/AshResult.h"
#include "GenesisMerge/GenesisMergeGit.hpp"
#include "GenesisShared/GenesisFlow.hpp"

namespace genesis::merge
{

    class GenesisFlowMerge
    {
    public:
        enum class FlowStatus : int
        {
            INVALID = 0,
            UNMODIFIED = 1,
            MODIFIED = 2,
            DELETED = 3,
            CREATED = 4
        };

        GenesisFlowMerge();
        GenesisFlowMerge(GenesisFlowMerge&) = delete;
        ~GenesisFlowMerge();

        void Reset();
        ash::AshResult Process(GenesisFlow* BaseFlow, GenesisFlow* LocalFlow, GenesisFlow* RemoteFlow);

        inline bool IsNeededToBeResolvedManually()
        {
            return m_IsNeededToBeResolvedManually;
        }

    private:
        ash::AshBuffer* m_LocalBuffer;
        ash::AshBuffer* m_RemoteBuffer;
        FlowStatus m_LocalStatus;
        FlowStatus m_RemoteStatus;
        bool m_IsNeededToBeResolvedManually;
    };

} // namespace genesis::merge

#endif // !_GENESISMERGEFLOW_HPP
