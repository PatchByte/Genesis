#ifndef _GENESISOPERATIONS_HPP
#define _GENESISOPERATIONS_HPP

#include "Ash/AshResult.h"
#include "Ash/AshStreamableObject.h"
#include "GenesisShared/GenesisLoadedFile.hpp"
#include <string>
#include <string_view>

namespace genesis::operations
{

    class GenesisOperationState
    {
    public:
        inline GenesisOperationState(common::GenesisLoadedFile* LoadedFile) : m_LoadedFile(LoadedFile), m_RawValue(0)
        {
        }

        inline void SetRawValue(unsigned long long Value)
        {
            m_RawValue = Value;
        }

        inline unsigned long long GetRawValue()
        {
            return m_RawValue;
        }

        inline void SetPointerValue(void* PointerValue)
        {
            m_PointerValue = PointerValue;
        }

        inline void* GetPointerValue()
        {
            return m_PointerValue;
        }

        template <typename T>
        T GetValueAs()
        {
            return reinterpret_cast<T>(m_PointerValue);
        }

        common::GenesisLoadedFile* GetLoadedFile()
        {
            return m_LoadedFile;
        }

    protected:
        // The Value is most likely a pointer which is the virtual address in the image.
        // Please note it is the **virtual address**, not the offset in the image.
        union
        {
            unsigned long long m_RawValue;
            void* m_PointerValue;
        };

        common::GenesisLoadedFile* m_LoadedFile;
    };

    class GenesisOperationInformation
    {
    public:
        // TODO implement methods
        // tho this is not needed because this structure is easily discardable

        bool m_DiscardsPreviousValue : 1;
        bool m_IsMathOperation : 1;
    };

    class GenesisBaseOperation : public ash::AshStreamableObject
    {
    public:
        ~GenesisBaseOperation() = default;

        virtual std::string GetOperationName()
        {
            return "GenesisBaseOperation";
        }

        virtual GenesisOperationInformation GetOperationInformation()
        {
            return GenesisOperationInformation();
        }

        virtual ash::AshResult ProcessOperation(GenesisOperationState* State)
        {
            return ash::AshResult(false, "Operation not implement");
        }
    };

    class GenesisFindPatternOperation : public GenesisBaseOperation
    {
    public:
        GenesisFindPatternOperation();

        std::string GetOperationName()
        {
            return "FindPatternOperation";
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

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
        enum class Type : unsigned char
        {
            INVALID = 0,
            ADDITION = 1,
            SUBTRACTION = 2
        };

        GenesisMathOperation();
        
        std::string GetOperationName()
        {
            return "MathOperation";
        }

        GenesisOperationInformation GetOperationInformation();
        ash::AshResult ProcessOperation(GenesisOperationState* State);

        virtual void SetType(Type Type)
        {
            m_Type = Type;
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

    class GenesisOperationUtils
    {
    public:
        static GenesisBaseOperation* sfCreateOperationByName(std::string OperationName);
    };

} // namespace genesis::operations

#endif // !_GENESISOPERATIONS_HPP