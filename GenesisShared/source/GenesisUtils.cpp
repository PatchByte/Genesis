#include "GenesisShared/GenesisUtils.hpp"

#if _WIN32
#include <windows.h> //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h> //readlink
#endif

namespace genesis
{
    std::filesystem::path GenesisUtils::GetOwnExectuablePath()
    {
#ifdef _WIN32
        wchar_t path[MAX_PATH] = {0};
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return path;
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
#endif
    }
} // namespace genesis
