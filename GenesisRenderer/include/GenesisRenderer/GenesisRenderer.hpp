#ifndef _GENESISRENDERER_HPP
#define _GENESISRENDERER_HPP

#include <functional>
#include <string>

namespace genesis::renderer
{

    class GenesisRendererBase
    {
    public:
        using sdDropFileHandler = std::function<void(int FilePathCount, const char** FilePathArray)>;

        GenesisRendererBase();
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

        virtual sdDropFileHandler GetDropFileHandler()
        {
            return m_DropFileHandler;
        }

        virtual void SetDropFileHandler(sdDropFileHandler DropFileHandler)
        {
            m_DropFileHandler = DropFileHandler;
        }

    protected:
        sdDropFileHandler m_DropFileHandler;
    };

    class GenesisRendererProvider
    {
    public:
        // Might do platform specific stuff in the future
        static GenesisRendererBase* CreateRenderer(unsigned Width, unsigned Height, std::string Title);
    };

} // namespace genesis::renderer

#endif // !_GENESISRENDERER_HPP