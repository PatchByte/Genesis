#include "GenesisShared/GenesisOperations.hpp"
#include <map>

#define IMPLEMENT_CREATION_DELEGATE(StringName, ClassName) { StringName, [] () -> GenesisBaseOperation* { return new ClassName (); } }

namespace genesis::operations 
{

    using sdOperationCreateDelegate = GenesisBaseOperation*();

    GenesisBaseOperation* GenesisOperationUtils::sfCreateOperationByName(std::string OperationName)
    {
        static std::map<std::string, sdOperationCreateDelegate*> smDelegates = {
            IMPLEMENT_CREATION_DELEGATE("FindPatternOperation", GenesisFindPatternOperation)
        };

        if(smDelegates.contains(OperationName))
        {
            return smDelegates.at(OperationName)();
        }

        return nullptr;
    }

}