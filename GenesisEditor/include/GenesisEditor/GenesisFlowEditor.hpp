#ifndef _GENESISFLOWEDITOR_HPP
#define _GENESISFLOWEDITOR_HPP

#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::editor
{

    class GenesisFlowEditor : public GenesisFlow
    {
    public:
        GenesisFlowEditor();
        ~GenesisFlowEditor();

        void Initialize();
        void Render();
        void RenderNodeOperation(operations::GenesisBaseOperation* Operation);
    };

}

#endif // !_GENESISFLOWEDITOR_HPP