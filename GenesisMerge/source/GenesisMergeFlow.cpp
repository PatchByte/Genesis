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
    static GenesisFlowMerge::FlowStatus sfProcessStatus(GenesisFlow* BaseFlow, GenesisFlow* TargetFlow, ash::AshBuffer** TargetBuffer, ash::AshCRC32Value* TargetCRC)
    {
        if (BaseFlow == nullptr)
        {
            if (TargetFlow == nullptr)
            {
                return GenesisFlowMerge::FlowStatus::UNMODIFIED;
            }
            else
            {
                return GenesisFlowMerge::FlowStatus::CREATED;
            }
        }
        else
        {
            if (TargetFlow == nullptr)
            {
                return GenesisFlowMerge::FlowStatus::DELETED;
            }
            else
            {
                ash::AshStreamExpandableExportBuffer baseStream = ash::AshStreamExpandableExportBuffer();
                ash::AshStreamExpandableExportBuffer targetStream = ash::AshStreamExpandableExportBuffer();

                if (BaseFlow->Export(&baseStream) == false || TargetFlow->Export(&targetStream) == false)
                {
                    return GenesisFlowMerge::FlowStatus::INVALID;
                }

                ash::AshBuffer* baseBuffer = baseStream.MakeCopyOfBuffer();

                *TargetBuffer = targetStream.MakeCopyOfBuffer();
                *TargetCRC = ash::AshCRC32Utils::Calculate(0, *TargetBuffer);

                ash::AshCRC32Value baseCRC = ash::AshCRC32Utils::Calculate(0, baseBuffer);

                GenesisFlowMerge::FlowStatus status;

                if(baseCRC == *TargetCRC)
                {
                    status = GenesisFlowMerge::FlowStatus::UNMODIFIED;
                }
                else
                {
                    printf("%x %x\n", baseCRC, *TargetCRC);

                    status = GenesisFlowMerge::FlowStatus::MODIFIED;
                }

                delete baseBuffer;

                return status;
            }
        }
    }

    // Flow Status Helper

    std::string GenesisFlowMerge::sfGetFlowStatusAsString(FlowStatus Status)
    {
        switch (Status)
        {
        case FlowStatus::UNMODIFIED:
            return "Unmodified";
        case FlowStatus::MODIFIED:
            return "Modified";
        case FlowStatus::DELETED:
            return "Deleted";
        case FlowStatus::CREATED:
            return "Created";
        default:
        case FlowStatus::INVALID:
            return "Invalid";
        }
    }

    GenesisFlowMerge::GenesisFlowMerge()
        : m_LocalBuffer(nullptr), m_RemoteBuffer(nullptr), m_LocalStatus(FlowStatus::INVALID), m_RemoteStatus(FlowStatus::INVALID), m_IsNeededToBeResolvedManually(false)
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

        printf(" local: ");
        m_LocalStatus = sfProcessStatus(BaseFlow, LocalFlow, &m_LocalBuffer, &localCRC);
        printf(" remote: ");
        m_RemoteStatus = sfProcessStatus(BaseFlow, RemoteFlow, &m_RemoteBuffer, &remoteCRC);

        if (m_LocalStatus == FlowStatus::INVALID || m_RemoteStatus == FlowStatus::INVALID)
        {
            return ash::AshResult(false, "Failed to compare.");
        }

        if (m_LocalStatus != FlowStatus::UNMODIFIED)
        {
            m_IsNeededToBeResolvedManually |= (m_RemoteStatus != FlowStatus::UNMODIFIED);
        }

        if (m_RemoteStatus != FlowStatus::UNMODIFIED)
        {
            m_IsNeededToBeResolvedManually |= (m_LocalStatus != FlowStatus::UNMODIFIED);
        }

        m_IsNeededToBeResolvedManually |= (localCRC != remoteCRC);

        return true;
    }

} // namespace genesis::merge
