#include "GenesisShared/GenesisOperations.hpp"
#include <map>

#define IMPLEMENT_CREATION_DELEGATE(StringName, ClassName)                    \
    {                                                                         \
        StringName, []() -> GenesisBaseOperation* { return new ClassName(); } \
    }

namespace genesis::operations
{

    using sdOperationCreateDelegate = GenesisBaseOperation*();

    GenesisBaseOperation* GenesisOperationUtils::sfCreateOperationByType(GenesisOperationType OperationType)
    {
        static std::map<GenesisOperationType, sdOperationCreateDelegate*> smDelegates = {
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::FIND_PATTERN, GenesisFindPatternOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::MATH, GenesisMathOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::DEBUG, GenesisDebugOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::GET, GenesisGetOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::OUTPUT_DATA_CLASS_MEMBER_VARIABLE, GenesisOutputDataClassMemberVariableOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::OUTPUT_DATA_CLASS_VTABLE_FUNCTION, GenesisOutputDataClassVTableFunctionOperation)};

        if (smDelegates.contains(OperationType))
        {
            return smDelegates.at(OperationType)();
        }

        return nullptr;
    }

} // namespace genesis::operations