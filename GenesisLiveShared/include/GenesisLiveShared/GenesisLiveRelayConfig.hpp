#ifndef _GENESISLIVERELAYCONFIG_HPP
#define _GENESISLIVERELAYCONFIG_HPP

#include <chrono>

namespace genesis::live
{

    class GenesisLiveRelayConfig
    {
    public:
        static constexpr std::chrono::milliseconds smTimeoutDuration = std::chrono::milliseconds(1500);
    };

}

#endif // !_GENESISLIVERELAYCONFIG_HPP