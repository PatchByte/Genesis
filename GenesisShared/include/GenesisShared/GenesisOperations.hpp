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

    enum class GenesisOperationType : int
    {
        INVALID = 0,
        FIND_PATTERN = 1,
        MATH = 2,
        DEBUG = 3,
        GET = 4,
        OUTPUT_DATA_CLASS_MEMBER_VARIABLE = 5,
        OUTPUT_DATA_CLASS_VIRTUAL_FUNCTION = 6,
        OUTPUT_DATA_CLASS_NON_VIRTUAL_FUNCTION = 7,
        RIP = 8,
        OUTPUT_DATA_STATIC_FUNCTION = 9,
        OUTPUT_DATA_STATIC_VARIABLE = 10,
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
            m_IsInteractOperation = false;
            m_IsOutputOperation = false;
            m_HasInputPin = false;
            m_HasOutputPin = false;
            m_InputPinId = 0;
            m_OutputPinId = 0;
        }

        bool m_DiscardsPreviousValue : 1;
        bool m_IsFlowStartNode : 1;
        bool m_IsConditionalFlowStartNode : 1;
        bool m_IsMathOperation : 1;
        bool m_IsInteractOperation : 1;
        bool m_IsOutputOperation : 1;
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

            operationInformation.m_InputPinId = utils::GenesisPinValue::Build(m_OperationId, 1, utils::GenesisPinType::INPUT).Get();
            operationInformation.m_OutputPinId = utils::GenesisPinValue::Build(m_OperationId, 2, utils::GenesisPinType::OUTPUT).Get();

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

    class GenesisGetOperation : public GenesisBaseOperation
    {
    public:
        enum class ValueKindType : int
        {
            UINT8 = 0,
            UINT16 = 1,
            UINT32 = 2,
            UINT64 = 3,

            // Signed types are disabled for now.
            // If you need signed types please let me know.
            // SINT8 = 4,
            // SINT16 = 5,
            // SINT32 = 6,
            // SINT64 = 7
        };

        GenesisGetOperation();
        GenesisGetOperation(ValueKindType ValueKind);

        std::string GetOperationName()
        {
            return "GetOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Get Value";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::GET;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline ValueKindType GetValueKind()
        {
            return m_ValueKind;
        }

        inline void SetValueKind(ValueKindType ValueKind)
        {
            m_ValueKind = ValueKind;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        ValueKindType m_ValueKind;
    };

    class GenesisRipOperation : public GenesisBaseOperation
    {
    public:
        GenesisRipOperation();
        GenesisRipOperation(unsigned char Carry);

        std::string GetOperationName()
        {
            return "RipOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Rip";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::RIP;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline unsigned char GetCarry()
        {
            return m_Carry;
        }

        inline void SetCarry(unsigned char Carry)
        {
            m_Carry = Carry;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        unsigned char m_Carry;
    };

    class GenesisOutputDataClassMemberVariableOperation : public GenesisBaseOperation
    {
    public:
        GenesisOutputDataClassMemberVariableOperation();
        GenesisOutputDataClassMemberVariableOperation(std::string ClassName, std::string MemberName, std::string TypeDefinition);

        std::string GetOperationName()
        {
            return "OutputDataClassMemberVariableOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Class Member Variable";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::OUTPUT_DATA_CLASS_MEMBER_VARIABLE;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline std::string GetClassName()
        {
            return m_ClassName;
        }

        inline void SetClassName(std::string ClassName)
        {
            m_ClassName = ClassName;
        }

        inline std::string GetMemberName()
        {
            return m_MemberName;
        }

        inline void SetMemberName(std::string MemberName)
        {
            m_MemberName = MemberName;
        }

        inline std::string GetTypeDefinition()
        {
            return m_TypeDefinition;
        }

        inline void SetTypeDefinition(std::string TypeDefinition)
        {
            m_TypeDefinition = TypeDefinition;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_ClassName;
        std::string m_MemberName;
        std::string m_TypeDefinition;
    };

    class GenesisOutputDataClassVirtualFunctionOperation : public GenesisBaseOperation
    {
    public:
        GenesisOutputDataClassVirtualFunctionOperation();
        GenesisOutputDataClassVirtualFunctionOperation(std::string ClassName, std::string MemberName);

        std::string GetOperationName()
        {
            return "OutputDataClassVirtualFunctionOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Class Virtual Function";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::OUTPUT_DATA_CLASS_VIRTUAL_FUNCTION;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline std::string GetClassName()
        {
            return m_ClassName;
        }

        inline void SetClassName(std::string ClassName)
        {
            m_ClassName = ClassName;
        }

        inline std::string GetFunctionName()
        {
            return m_FunctionName;
        }

        inline void SetFunctionName(std::string FunctionName)
        {
            m_FunctionName = FunctionName;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_ClassName;
        std::string m_FunctionName;
    };

    class GenesisOutputDataClassNonVirtualFunctionOperation : public GenesisBaseOperation
    {
    public:
        GenesisOutputDataClassNonVirtualFunctionOperation();
        GenesisOutputDataClassNonVirtualFunctionOperation(std::string ClassName, std::string MemberName);

        std::string GetOperationName()
        {
            return "OutputDataClassNonVirtualFunctionOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Class Non Virtual Function";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::OUTPUT_DATA_CLASS_NON_VIRTUAL_FUNCTION;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline std::string GetClassName()
        {
            return m_ClassName;
        }

        inline void SetClassName(std::string ClassName)
        {
            m_ClassName = ClassName;
        }

        inline std::string GetFunctionName()
        {
            return m_FunctionName;
        }

        inline void SetFunctionName(std::string FunctionName)
        {
            m_FunctionName = FunctionName;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_ClassName;
        std::string m_FunctionName;
    };

    class GenesisOutputDataStaticFunctionOperation : public GenesisBaseOperation
    {
    public:
        GenesisOutputDataStaticFunctionOperation();
        // Static Non-Class
        GenesisOutputDataStaticFunctionOperation(std::string FunctionName);
        // Static Class
        GenesisOutputDataStaticFunctionOperation(std::string ClassName, std::string MemberName);

        std::string GetOperationName()
        {
            return "GenesisOutputDataStaticFunctionOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Static Function";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::OUTPUT_DATA_STATIC_FUNCTION;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline std::string GetClassName()
        {
            return m_ClassName;
        }

        inline void SetClassName(std::string ClassName)
        {
            m_ClassName = ClassName;
        }

        inline bool IsInClass()
        {
            return m_ClassName.empty() == false;
        }

        inline bool IsGlobal()
        {
            return m_ClassName.empty() == true;
        }

        inline std::string GetFunctionName()
        {
            return m_FunctionName;
        }

        inline void SetFunctionName(std::string FunctionName)
        {
            m_FunctionName = FunctionName;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_ClassName;
        std::string m_FunctionName;
    };

    class GenesisOutputDataStaticVariableOperation : public GenesisBaseOperation
    {
    public:
        GenesisOutputDataStaticVariableOperation();
        // Static Non-Class
        GenesisOutputDataStaticVariableOperation(std::string VariableName);
        // Static Class
        GenesisOutputDataStaticVariableOperation(std::string ClassName, std::string VariableName);

        std::string GetOperationName()
        {
            return "OutputDataStaticVariableOperation";
        }

        std::string GetHumanReadableName()
        {
            return "Static Variable";
        }

        GenesisOperationType GetOperationType()
        {
            return GenesisOperationType::OUTPUT_DATA_STATIC_VARIABLE;
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        inline std::string GetClassName()
        {
            return m_ClassName;
        }

        inline void SetClassName(std::string ClassName)
        {
            m_ClassName = ClassName;
        }

        inline bool IsInClass()
        {
            return m_ClassName.empty() == false;
        }

        inline bool IsGlobal()
        {
            return m_ClassName.empty() == true;
        }

        inline std::string GetVariableName()
        {
            return m_VariableName;
        }

        inline void SetVariableName(std::string VariableName)
        {
            m_VariableName = VariableName;
        }

        bool Import(ash::AshStream* Stream);
        bool Export(ash::AshStream* Stream);

    protected:
        std::string m_ClassName;
        std::string m_VariableName;
    };

    // Utils

    class GenesisOperationUtils
    {
    public:
        static GenesisBaseOperation* sfCreateOperationByType(GenesisOperationType OperationType);
    };

} // namespace genesis::operations

#endif // !_GENESISOPERATIONS_HPP