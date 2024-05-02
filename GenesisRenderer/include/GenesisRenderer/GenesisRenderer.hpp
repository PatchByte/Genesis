#ifndef _GENESISRENDERER_HPP
#define _GENESISRENDERER_HPP

#include <string>

namespace genesis::renderer
{

    class GenesisRendererBase
    {
    public:
        virtual ~GenesisRendererBase() = default;

        virtual bool Initialize()
        {
            return false;
        }

        virtual bool Shutdown()
        {
            return false;
        }

        virtual bool BeginFrame()
        {
            return false;
        }

        virtual bool EndFrame()
        {
            return false;
        }

        virtual bool ShallRender()
        {
            return false;
        }
    };

    class GenesisRendererProvider
    {
    public:
        // Might do platform specific stuff in the future
        static GenesisRendererBase* CreateRenderer(unsigned Width, unsigned Height, std::string Title);
    };

} // namespace genesis::renderer

#endif // !_GENESISRENDERER_HPP