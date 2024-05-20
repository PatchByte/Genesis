#ifndef _GENESISOUTPUTBUILDER_HPP
#define _GENESISOUTPUTBUILDER_HPP

#include "GenesisShared/GenesisOutput.hpp"
#include <string>

namespace genesis::output
{

    class GenesisOutputBuilder
    {
    public:
        static std::string BuildClassInner(GenesisOutputClass* Class);
        static std::string Build(GenesisOutputData* OutputData); 
    };

}

#endif // !_GENESISOUTPUTFORMATTER_HPP