#ifndef _GENESISOPERATIONSEDITOR_HPP
#define _GENESISOPERATIONSEDITOR_HPP

#include "GenesisShared/GenesisOperations.hpp"

namespace genesis::editor 
{

    class GenesisOperationEditorForNodes
    {
    public:
        static float sfGetMaxItemWidth(std::string Text);
        static void sfRenderOperation(operations::GenesisBaseOperation* Operation);
        static void sfRenderFindPatternOperation(operations::GenesisFindPatternOperation* Operation, const operations::GenesisOperationInformation& OperationInfo);
        static void sfRenderMathOperation(operations::GenesisMathOperation* Operation, const operations::GenesisOperationInformation& OperationInfo);
    };

}

#endif // !_GENESISOPERATIONSEDITOR_HPP