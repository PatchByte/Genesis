#include "GenesisMerge/Main.hpp"
#include "GenesisMerge/GenesisMerge.hpp"

int main(int argc, const char** argv)
{
    genesis::merge::GenesisMerge merge = genesis::merge::GenesisMerge();
    return static_cast<int>(merge.Run(argc, argv));
}
