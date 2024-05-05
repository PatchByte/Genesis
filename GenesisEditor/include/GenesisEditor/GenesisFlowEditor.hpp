#ifndef _GENESISFLOWEDITOR_HPP
#define _GENESISFLOWEDITOR_HPP

#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::editor
{

    class GenesisFlowEditor
    {
    public:
        GenesisFlowEditor(GenesisFlow* Flow);
        ~GenesisFlowEditor();

        void Initialize();
        void Render();
        void RenderNodeOperation(GenesisFlowIndex Index, operations::GenesisBaseOperation* Operation);

        inline GenesisFlow* GetFlow() { return m_Flow; }
    private:
        GenesisFlow* m_Flow;
        std::vector<std::pair<int, int>> m_Links;
    };

}

#endif // !_GENESISFLOWEDITOR_HPP