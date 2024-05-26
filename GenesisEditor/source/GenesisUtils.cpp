#include "GenesisEditor/GenesisUtils.hpp"

#ifdef linux  
#include <unistd.h>  
#include <pwd.h>  
#endif  

#ifdef _WIN32  
#include<Windows.h>  
#endif  

namespace genesis::utils
{

    std::string GenesisUtils::sfGetUsername()
    {
#if linux
        uid_t userid;
        struct passwd* pwd;
        userid = getuid();
        pwd = getpwuid(userid);
        return pwd->pw_name;

#elif _WIN32
        const int MAX_LEN = 100;
        char szBuffer[MAX_LEN];
        DWORD len = MAX_LEN;
        if (GetUserName(szBuffer, &len))
            return szBuffer;

#else
        return "";
#endif
    }

} // namespace genesis::utils