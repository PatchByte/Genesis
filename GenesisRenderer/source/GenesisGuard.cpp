#include "GenesisRenderer/GenesisGuard.hpp"
#include "GLFW/glfw3.h"
#include <cstdio>
#include <cstdlib>

namespace genesis::renderer
{

    GenesisGuard::GenesisGuard()
    {
        glfwSetErrorCallback(
            [](int ErrorCode, const char* Description) -> void
            {
                printf("[!] GLFW Error: %i (%s)\n", ErrorCode, Description ? Description : "No Description");
                exit(-1);
            });

        glfwInit();
    }

    GenesisGuard::~GenesisGuard()
    {
        glfwTerminate();
    }

} // namespace genesis::renderer