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
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::RIP, GenesisRipOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::OUTPUT_DATA_CLASS_MEMBER_VARIABLE, GenesisOutputDataClassMemberVariableOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::OUTPUT_DATA_CLASS_VIRTUAL_FUNCTION, GenesisOutputDataClassVirtualFunctionOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::OUTPUT_DATA_CLASS_NON_VIRTUAL_FUNCTION, GenesisOutputDataClassNonVirtualFunctionOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::OUTPUT_DATA_STATIC_FUNCTION, GenesisOutputDataStaticFunctionOperation),
            IMPLEMENT_CREATION_DELEGATE(GenesisOperationType::OUTPUT_DATA_STATIC_VARIABLE, GenesisOutputDataStaticVariableOperation)};

        if (smDelegates.contains(OperationType))
        {
            return smDelegates.at(OperationType)();
        }

        return nullptr;
    }

} // namespace genesis::operations