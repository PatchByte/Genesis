#ifndef _GENESISUTILS_HPP
#define _GENESISUTILS_HPP

#include <filesystem>

namespace genesis
{

    class GenesisUtils
    {
    public:
        static std::filesystem::path GetOwnExectuablePath();
    };
} // namespace genesis

#endif // !_GENESISUTILS_HPP
