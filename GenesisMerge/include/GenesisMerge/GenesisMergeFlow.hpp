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

        enum class ResolvementStatus : int
        {
            UNRESOLVED = 0,
            TAKE_REMOTE = 2,
            TAKE_LOCAL = 3
        };

        static std::string sfGetFlowStatusAsString(FlowStatus Status);

        GenesisFlowMerge();
        GenesisFlowMerge(GenesisFlowMerge&) = delete;
        ~GenesisFlowMerge();

        void Reset();
        ash::AshResult Process(GenesisFlow* BaseFlow, GenesisFlow* LocalFlow, GenesisFlow* RemoteFlow);

        inline FlowStatus GetLocalStatus()
        {
            return m_LocalStatus;
        }

        inline FlowStatus GetRemoteStatus()
        {
            return m_RemoteStatus;
        }

        inline bool IsNeededToBeResolvedManually()
        {
            return m_IsNeededToBeResolvedManually;
        }

        inline ResolvementStatus GetManualResolvementStatus()
        {
            return m_ManualResolvementStatus;
        }

        inline void SetManualResolvementStatus(ResolvementStatus ManualResolvementStatus)
        {
            m_ManualResolvementStatus = ManualResolvementStatus;
        }

        bool IsResolved();

    private:
        ash::AshBuffer* m_LocalBuffer;
        ash::AshBuffer* m_RemoteBuffer;
        FlowStatus m_LocalStatus;
        FlowStatus m_RemoteStatus;
        bool m_IsNeededToBeResolvedManually;
        ResolvementStatus m_ManualResolvementStatus;
    };

} // namespace genesis::merge

#endif // !_GENESISMERGEFLOW_HPP
