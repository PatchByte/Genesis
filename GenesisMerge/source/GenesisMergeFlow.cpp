#include "GenesisMerge/GenesisMergeFlow.hpp"
#include "Ash/AshBuffer.h"
#include "Ash/AshCRC32.h"
#include "Ash/AshResult.h"
#include "Ash/AshStream.h"

namespace genesis::merge
{
    /*!
     * @brief An algorithm to check if the base flow and target flow are the same, and if something has changed, what has changed.
     *
     * @param[out] TargetCRC The calculated TargetFlow crc
     * @param[out] TargetBuffer The allocated export buffer of TargetFlow, needs to be manually deallocated.
     */
    static GenesisFlowMerge::FlowHistorialStatus sfProcessStatus(GenesisFlow* BaseFlow, GenesisFlow* TargetFlow, ash::AshBuffer** TargetBuffer, ash::AshCRC32Value* TargetCRC)
    {
        if (BaseFlow == nullptr)
        {
            if (TargetFlow == nullptr)
            {
                return GenesisFlowMerge::FlowHistorialStatus::UNMODIFIED;
            }
            else
            {
                ash::AshStreamExpandableExportBuffer targetStream = ash::AshStreamExpandableExportBuffer();

                if (TargetFlow->Export(&targetStream) == false)
                {
                    return GenesisFlowMerge::FlowHistorialStatus::INVALID;
                }

                *TargetBuffer = targetStream.MakeCopyOfBuffer();
                *TargetCRC = ash::AshCRC32Utils::Calculate(0, *TargetBuffer);

                return GenesisFlowMerge::FlowHistorialStatus::CREATED;
            }
        }
        else
        {
            if (TargetFlow == nullptr)
            {
                return GenesisFlowMerge::FlowHistorialStatus::DELETED;
            }
            else
            {
                ash::AshStreamExpandableExportBuffer baseStream = ash::AshStreamExpandableExportBuffer();
                ash::AshStreamExpandableExportBuffer targetStream = ash::AshStreamExpandableExportBuffer();

                if (BaseFlow->Export(&baseStream) == false || TargetFlow->Export(&targetStream) == false)
                {
                    return GenesisFlowMerge::FlowHistorialStatus::INVALID;
                }

                ash::AshBuffer* baseBuffer = baseStream.MakeCopyOfBuffer();

                *TargetBuffer = targetStream.MakeCopyOfBuffer();
                *TargetCRC = ash::AshCRC32Utils::Calculate(0, *TargetBuffer);

                ash::AshCRC32Value baseCRC = ash::AshCRC32Utils::Calculate(0, baseBuffer);

                GenesisFlowMerge::FlowHistorialStatus status;

                if (baseCRC == *TargetCRC)
                {
                    status = GenesisFlowMerge::FlowHistorialStatus::UNMODIFIED;
                }
                else
                {
                    status = GenesisFlowMerge::FlowHistorialStatus::MODIFIED;
                }

                delete baseBuffer;

                return status;
            }
        }
    }

    // Flow Status Helper

    std::string GenesisFlowMerge::sfGetFlowStatusAsString(FlowHistorialStatus Status)
    {
        switch (Status)
        {
        case FlowHistorialStatus::UNMODIFIED:
            return "Unmodified";
        case FlowHistorialStatus::MODIFIED:
            return "Modified";
        case FlowHistorialStatus::DELETED:
            return "Deleted";
        case FlowHistorialStatus::CREATED:
            return "Created";
        default:
        case FlowHistorialStatus::INVALID:
            return "Invalid";
        }
    }

    std::string GenesisFlowMerge::sfGetResolvementStatusAsLabel(FlowManualResolvementStatus Status)
    {
        switch (Status)
        {
        case GenesisFlowMerge::FlowManualResolvementStatus::TAKE_REMOTE:
            return "Taking Remote";
        case GenesisFlowMerge::FlowManualResolvementStatus::TAKE_LOCAL:
            return "Taking Local";
        default:
        case GenesisFlowMerge::FlowManualResolvementStatus::UNRESOLVED:
            return "Unresolved";
        }
    }

    GenesisFlowMerge::GenesisFlowMerge()
        : m_LocalBuffer(nullptr), m_RemoteBuffer(nullptr), m_LocalStatus(FlowHistorialStatus::INVALID), m_RemoteStatus(FlowHistorialStatus::INVALID), m_IsNeededToBeResolvedManually(false),
          m_ManualResolvementStatus(GenesisFlowMerge::FlowManualResolvementStatus::UNRESOLVED)
    {
    }

    GenesisFlowMerge::~GenesisFlowMerge()
    {
        Reset();
    }

    void GenesisFlowMerge::Reset()
    {
        if (m_LocalBuffer)
        {
            delete m_LocalBuffer;
            m_LocalBuffer = nullptr;
        }

        if (m_RemoteBuffer)
        {
            delete m_RemoteBuffer;
            m_RemoteBuffer = nullptr;
        }

        m_IsNeededToBeResolvedManually = false;
    }

    ash::AshResult GenesisFlowMerge::Process(GenesisFlow* BaseFlow, GenesisFlow* LocalFlow, GenesisFlow* RemoteFlow)
    {
        Reset();

        ash::AshCRC32Value localCRC = 0, remoteCRC = 0;

        m_LocalStatus = sfProcessStatus(BaseFlow, LocalFlow, &m_LocalBuffer, &localCRC);
        m_RemoteStatus = sfProcessStatus(BaseFlow, RemoteFlow, &m_RemoteBuffer, &remoteCRC);

        if (m_LocalStatus == FlowHistorialStatus::INVALID || m_RemoteStatus == FlowHistorialStatus::INVALID)
        {
            return ash::AshResult(false, "Failed to compare.");
        }

        if (m_LocalStatus != FlowHistorialStatus::UNMODIFIED)
        {
            m_IsNeededToBeResolvedManually |= (m_RemoteStatus != FlowHistorialStatus::UNMODIFIED);
        }

        if (m_RemoteStatus != FlowHistorialStatus::UNMODIFIED)
        {
            m_IsNeededToBeResolvedManually |= (m_LocalStatus != FlowHistorialStatus::UNMODIFIED);
        }

        if (m_IsNeededToBeResolvedManually == false)
        {
            if (m_LocalStatus == FlowHistorialStatus::UNMODIFIED && m_RemoteStatus == FlowHistorialStatus::UNMODIFIED)
            {
                m_ManualResolvementStatus = FlowManualResolvementStatus::TAKE_LOCAL;
            }
            else if (m_LocalStatus == FlowHistorialStatus::UNMODIFIED)
            {
                m_ManualResolvementStatus = FlowManualResolvementStatus::TAKE_REMOTE;
            }
            else if (m_RemoteStatus == FlowHistorialStatus::UNMODIFIED)
            {
                m_ManualResolvementStatus = FlowManualResolvementStatus::TAKE_LOCAL;
            }
        }

        // m_IsNeededToBeResolvedManually |= (localCRC != remoteCRC);

        return true;
    }

    bool GenesisFlowMerge::IsResolved()
    {
        if (IsNeededToBeResolvedManually())
        {
            return m_ManualResolvementStatus != FlowManualResolvementStatus::UNRESOLVED;
        }

        return true;
    }

} // namespace genesis::merge
