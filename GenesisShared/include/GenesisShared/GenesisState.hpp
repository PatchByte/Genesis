#ifndef _GENESISSTATE_HPP
#define _GENESISSTATE_HPP

#include "GenesisShared/GenesisLoadedFile.hpp"

namespace genesis
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

} // namespace genesis

#endif // !_GENESISSTATE_HPP