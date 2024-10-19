#include "GenesisOutput/GenesisOutputBuilder.hpp"
#include "GenesisShared/GenesisOutput.hpp"
#include "fmt/format.h"
#include <cstdlib>
#include <ctime>
#include <ostream>
#include <sstream>

namespace genesis::output
{

    std::string GenesisOutputBuilder::BuildClassInner(GenesisOutputClass* Class)
    {
        std::stringstream outputStream = std::stringstream();

        // Virtual Functions

        outputStream << "class VirtualFunctions" << std::endl;
        outputStream << "{" << std::endl;
        outputStream << "public:" << std::endl;

        for (auto currentVirtualFunction : Class->GetVirtualFunctions())
        {
            outputStream << "static constexpr unsigned long long " << currentVirtualFunction.second.GetName() << " = " << std::hex << "0x" << currentVirtualFunction.second.GetVirtualTableOffset()
                         << ";" << std::dec << std::endl;
        }

        outputStream << "};" << std::endl;
        outputStream << std::endl;

        // Non-Virtual Functions

        outputStream << "class NonVirtualFunctions" << std::endl;
        outputStream << "{" << std::endl;
        outputStream << "public:" << std::endl;
        for (auto currentNonVirtualFunction : Class->GetNonVirtualFunctions())
        {
            outputStream << "static constexpr unsigned long long " << currentNonVirtualFunction.second.GetName() << " = " << std::hex << "0x" << currentNonVirtualFunction.second.GetOffset() << ";"
                         << std::dec << std::endl;
        }
        outputStream << "};" << std::endl;
        outputStream << std::endl;

        // Static Functions

        outputStream << "class StaticFunctions" << std::endl;
        outputStream << "{" << std::endl;
        outputStream << "public:" << std::endl;
        for (auto currentStaticFunction : Class->GetStaticFunctions())
        {
            outputStream << "static constexpr unsigned long long " << currentStaticFunction.second.GetName() << " = " << std::hex << "0x" << currentStaticFunction.second.GetOffset() << ";" << std::dec
                         << std::endl;
        }
        outputStream << "};" << std::endl;
        outputStream << std::endl;

        // Static Functions

        outputStream << "class StaticVariables" << std::endl;
        outputStream << "{" << std::endl;
        outputStream << "public:" << std::endl;
        for (auto currentStaticVariables : Class->GetStaticVariables())
        {
            outputStream << "static constexpr unsigned long long " << currentStaticVariables.second.GetName() << " = " << std::hex << "0x" << currentStaticVariables.second.GetOffset() << ";"
                         << std::dec << std::endl;
        }
        outputStream << "};" << std::endl;
        outputStream << std::endl;

        // Members

        outputStream << "class Members" << std::endl;
        outputStream << "{" << std::endl;
        outputStream << "public:" << std::endl;

        for (auto currentMember : Class->GetMembers())
        {
            outputStream << "static constexpr unsigned long long " << currentMember.second.GetName() << " = " << std::hex << "0x" << currentMember.second.GetOffset() << ";" << std::dec << std::endl;
        }

        outputStream << "};" << std::endl;
        outputStream << std::endl;

        return outputStream.str();
    }

    std::string GenesisOutputBuilder::Build(GenesisOutputData* OutputData)
    {
        std::stringstream outputStream = std::stringstream();

        // Very bad way.
        srand(time(nullptr));
        std::string headerGuardName = fmt::format("_GENESIS_OUTPUT_{}", rand());

        outputStream << "#ifndef " << headerGuardName << std::endl;
        outputStream << "#define " << headerGuardName << std::endl;
        outputStream << std::endl;
        outputStream << "// Generated at: " << time(nullptr) << std::endl;
        outputStream << std::endl;
        outputStream << "namespace genesis::output" << std::endl;
        outputStream << "{" << std::endl;

        for (std::string currentClassName : OutputData->GetAllAvailableClassNames())
        {
            GenesisOutputClass* currentClass = OutputData->GetOrCreateClass(currentClassName);

            outputStream << "class " << currentClassName << std::endl;
            outputStream << "{" << std::endl;
            outputStream << "public:" << std::endl;

            outputStream << BuildClassInner(currentClass);

            outputStream << "};" << std::endl;
            outputStream << std::endl;
        }

        // Global

        {
            GenesisOutputClass* globalClass = OutputData->GetGlobalClass();

            outputStream << "class " << "OffsetProviderGlobal" << std::endl;
            outputStream << "{" << std::endl;
            outputStream << "public:" << std::endl;

            outputStream << BuildClassInner(globalClass);

            outputStream << "};" << std::endl;
            outputStream << std::endl;
        }

        outputStream << "}" << std::endl;
        outputStream << "#endif // !" << headerGuardName << std::endl;

        return outputStream.str();
    }

} // namespace genesis::output
