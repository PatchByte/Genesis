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
        enum class FlowHistorialStatus : int
        {
            INVALID = 0,
            UNMODIFIED = 1,
            MODIFIED = 2,
            DELETED = 3,
            CREATED = 4
        };

        enum class FlowManualResolvementStatus : int
        {
            UNRESOLVED = 0,
            TAKE_REMOTE = 2,
            TAKE_LOCAL = 3
        };

        static std::string sfGetFlowStatusAsString(FlowHistorialStatus Status);
        static std::string sfGetResolvementStatusAsLabel(FlowManualResolvementStatus Status);

        GenesisFlowMerge();
        GenesisFlowMerge(GenesisFlowMerge&) = delete;
        ~GenesisFlowMerge();

        void Reset();
        ash::AshResult Process(GenesisFlow* BaseFlow, GenesisFlow* LocalFlow, GenesisFlow* RemoteFlow);

        inline FlowHistorialStatus GetLocalStatus()
        {
            return m_LocalStatus;
        }

        inline FlowHistorialStatus GetRemoteStatus()
        {
            return m_RemoteStatus;
        }

        inline bool IsNeededToBeResolvedManually()
        {
            return m_IsNeededToBeResolvedManually;
        }

        inline FlowManualResolvementStatus GetManualResolvementStatus()
        {
            return m_ManualResolvementStatus;
        }

        inline void SetManualResolvementStatus(FlowManualResolvementStatus ManualResolvementStatus)
        {
            m_ManualResolvementStatus = ManualResolvementStatus;
        }

        bool IsResolved();

        inline FlowHistorialStatus GetResolvedFlowStatus()
        {
            if (m_ManualResolvementStatus == FlowManualResolvementStatus::TAKE_LOCAL)
            {
                return GetLocalStatus();
            }
            else if (m_ManualResolvementStatus == FlowManualResolvementStatus::TAKE_REMOTE)
            {
                return GetRemoteStatus();
            }

            return FlowHistorialStatus::INVALID;
        }

        inline ash::AshBuffer* GetResolvedBuffer()
        {
            if (m_ManualResolvementStatus == FlowManualResolvementStatus::TAKE_LOCAL)
            {
                return m_LocalBuffer;
            }
            else if (m_ManualResolvementStatus == FlowManualResolvementStatus::TAKE_REMOTE)
            {
                return m_RemoteBuffer;
            }

            return nullptr;
        }

    private:
        ash::AshBuffer* m_LocalBuffer;
        ash::AshBuffer* m_RemoteBuffer;
        FlowHistorialStatus m_LocalStatus;
        FlowHistorialStatus m_RemoteStatus;
        bool m_IsNeededToBeResolvedManually;
        FlowManualResolvementStatus m_ManualResolvementStatus;
    };

} // namespace genesis::merge

#endif // !_GENESISMERGEFLOW_HPP
