#ifndef _GENESISOPERATIONSEDITOR_HPP
#define _GENESISOPERATIONSEDITOR_HPP

#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::editor 
{

    class GenesisOperationEditorForNodes
    {
    public:
        static void sfRenderOperation(operations::GenesisBaseOperation* Operation);
        static void sfRenderFindPatternOperation(operations::GenesisFindPatternOperation* Operation);
        static void sfRenderMathOperation(operations::GenesisMathOperation* Operation);
    };

}

#endif // !_GENESISOPERATIONSEDITOR_HPP