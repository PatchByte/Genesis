#ifndef _GENESISOPERATIONS_HPP
#define _GENESISOPERATIONS_HPP

#include "Ash/AshResult.h"
#include "Ash/AshStreamableObject.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include "GenesisShared/GenesisPinTracker.hpp"
#include "GenesisShared/GenesisState.hpp"
#include <string>
#include <string_view>

namespace genesis::operations
{

    enum class GenesisOperationType
    {
        INVALID = 0,
        FIND_PATTERN = 1,
        MATH = 2,
        DEBUG = 3,
    };

    class GenesisOperationInformation
    {
    public:
        inline GenesisOperationInformation()
        {
            m_DiscardsPreviousValue = false;
            m_IsFlowStartNode = false;
            m_IsConditionalFlowStartNode = false;
            m_IsMathOperation = false;
            m_HasInputPin = false;
            m_HasOutputPin = false;
            m_InputPinId = 0;
            m_OutputPinId = 0;
        }

        bool m_DiscardsPreviousValue : 1;
        bool m_IsFlowStartNode : 1;
        bool m_IsConditionalFlowStartNode : 1;
        bool m_IsMathOperation : 1;
        bool m_HasInputPin : 1;
        bool m_HasOutputPin : 1;

        uintptr_t m_InputPinId;
        uintptr_t m_OutputPinId;
    };

    using GenesisOperationId = unsigned int;

    class GenesisBaseOperation : public ash::AshStreamableObject
    {
    public:
        ~GenesisBaseOperation() = default;

        virtual std::string GetOperationName()
        {
            return "GenesisBaseOperation";
        }

        virtual std::string GetHumanReadableName()
        {
            return "Unimplemented";
        }

        virtual GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::INVALID;
        }

        virtual GenesisOperationId GetOperationId()
        {
            return m_OperationId;
        }

        virtual void SetOperationId(GenesisOperationId OperationId)
        {
            m_OperationId = OperationId;
        }

        virtual GenesisOperationInformation GetOperationInformation()
        {
            GenesisOperationInformation operationInformation = {};

            operationInformation.m_InputPinId = utils::GenesisPinValue(m_OperationId, 1, utils::GenesisPinType::INPUT).Get();
            operationInformation.m_OutputPinId = utils::GenesisPinValue(m_OperationId, 2, utils::GenesisPinType::OUTPUT).Get();

            return std::move(operationInformation);
        }

        virtual ash::AshResult ProcessOperation(GenesisOperationState* State)
        {
            return ash::AshResult(false, "Operation not implement");
        }

    protected:
        GenesisOperationId m_OperationId;
    };

    class GenesisFindPatternOperation : public GenesisBaseOperation
    {
    public:
        GenesisFindPatternOperation();
        GenesisFindPatternOperation(std::string Pattern);

        std::string GetOperationName()
        {
            return "FindPatternOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Pattern";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::FIND_PATTERN;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        virtual std::string GetPattern()
        {
            return m_Pattern;
        }

        virtual void SetPattern(std::string Pattern)
        {
            m_Pattern = Pattern;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_Pattern;
    };

    class GenesisMathOperation : public GenesisBaseOperation
    {
    public:
        enum class Type : int
        {
            INVALID = 0,
            ADDITION = 1,
            SUBTRACTION = 2
        };

        GenesisMathOperation();
        GenesisMathOperation(Type MathType, unsigned long long MathValue);

        std::string GetOperationName()
        {
            return "MathOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Math";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::MATH;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        Type GetType()
        {
            return m_Type;
        }

        virtual void SetType(Type Type)
        {
            m_Type = Type;
        }

        unsigned long long GetValue()
        {
            return m_Value;
        }

        virtual void SetValue(unsigned long long Value)
        {
            m_Value = Value;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        Type m_Type;
        unsigned long long m_Value;
    };

    class GenesisDebugOperation : public GenesisBaseOperation
    {
    public:
        GenesisDebugOperation();
        GenesisDebugOperation(std::string DebugMessage);

        std::string GetOperationName()
        {
            return "DebugOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Debug";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::DEBUG;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline std::string GetDebugMessage()
        {
            return m_DebugMessage;
        }

        inline void SetDebugMessage(std::string DebugMessage)
        {
            m_DebugMessage = DebugMessage;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_DebugMessage;
    };

    // Utils

    class GenesisOperationUtils
    {
    public:
        static GenesisBaseOperation* sfCreateOperationByType(GenesisOperationType OperationType);
    };

} // namespace genesis::operations

#endif // !_GENESISOPERATIONS_HPP