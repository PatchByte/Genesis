#include "generate.h"
#include "stdafx.h"
#include "temp.h"

#include "utils/io.h"
#include "utils/string.h"

int main(int argc, char* argv[])
{
    std::string gen_file_path = utils::path("src/temp.h");

#ifdef GENESIS
    run_static_replacement();
    std::cout << "[GENESIS] Succesfuly statically replaced entries" << std::endl;
    return utils::empty_file(gen_file_path) ? 0 : 1;
#else
    if (argc < 3)
    {
        return 1;
    }

    if (!utils::empty_file(gen_file_path) || !generate_temp_file(gen_file_path, argv[1], argv[2]))
    {
        std::cout << "failed to generate the temp file" << std::endl;
        return 1;
    }

    return std::system("xmake build genesis && xmake run genesis");
#endif
}
