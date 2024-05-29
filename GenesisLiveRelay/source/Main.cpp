#include "GenesisLiveRelay/Main.hpp"
#include "GenesisLiveRelay/GenesisLiveRelay.hpp"

int main(int argc, char** argv)
{

    genesis::live::GenesisLiveRelay relay = genesis::live::GenesisLiveRelay();
    relay.Run();

    return 0;
}