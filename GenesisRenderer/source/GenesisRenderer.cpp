#include "GenesisRenderer/GenesisRenderer.hpp"
#include "GenesisRenderer/GenesisGuard.hpp"
#include "GenesisRenderer/GenesisVulkanRenderer.hpp"

namespace genesis::renderer
{

    GenesisRendererBase* GenesisRendererProvider::CreateRenderer(unsigned Width, unsigned Height, std::string Title)
    {
        static GenesisGuard guard;

        return new GenesisVulkanImplementation(Width, Height, Title);
    }

}