#include "GenesisOutput/GenesisOutputBuilder.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include <ctime>
#include <ostream>
#include <sstream>

namespace genesis::output
{

    std::string GenesisOutputBuilder::Build(GenesisOutputData* OutputData)
    {
        // You want indentation?
        // Use clang-formatter blyat.

        std::stringstream outputStream = std::stringstream();

        outputStream << "#ifndef __GENESISOUTPUT_HPP" << std::endl;
        outputStream << "#define __GENESISOUTPUT_HPP" << std::endl;
        outputStream << std::endl;
        outputStream << "// This is an automatic generated file, do not change." << std::endl;
        outputStream << "// Generated at: " << time(nullptr) << std::endl;
        outputStream << std::endl;
        outputStream << "namespace genesis::generated" << std::endl;
        outputStream << "{" << std::endl;

        for (std::string currentClassName : OutputData->GetAllAvailableClassNames())
        {
            GenesisOutputClass* currentClass = OutputData->GetOrCreateClass(currentClassName);

            outputStream << "class " << "OffsetProvider" << currentClassName << std::endl;
            outputStream << "{" << std::endl;
            outputStream << "public:" << std::endl;

            // Members
            
            outputStream << "class Members" << std::endl;
            outputStream << "{" << std::endl;
            outputStream << "public:" << std::endl;
            for (auto currentMember : currentClass->GetMembers())
            {
                outputStream << "static constexpr " << currentMember.second.GetName() << " = " << std::hex << "0x" << currentMember.second.GetOffset() << ";" << std::dec << std::endl;
            }
            outputStream << "};" << std::endl;

            outputStream << "};" << std::endl;
            outputStream << std::endl;
        }

        outputStream << "}" << std::endl;
        outputStream << "#endif // !__GENESISOUTPUT_HPP" << std::endl;

        return outputStream.str();
    }

} // namespace genesis::output