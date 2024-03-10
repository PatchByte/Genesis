#include <filesystem>

#include "generate.h"
#include "utils/io.h"
#include "utils/string.h"

bool parse_entries(std::vector<Entry>& entries, const std::string& file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        printf("error opening file %s\n", file_path);
        return false;
    }

    std::string current_namespace;
    std::string line;

    bool look_for_pattern = false;
    uint16_t line_count = 1;

    while (std::getline(file, line))
    {
        if (std::string _namespace = utils::extract_namespace(line); _namespace != "")
        {
            // std::cout << "ns: " << line << std::endl;
            current_namespace = _namespace;
        }
        else if (look_for_pattern && utils::str_contains(line, "Memory::Pattern"))
        {
            // std::cout << "pat: " << line << std::endl;
            entries.at(entries.size() - 1).pattern = utils::extract_str(line);
        }
        else if (look_for_pattern && utils::str_contains(line, " = "))
        {
            // std::cout << "scan: " << line << std::endl;
            entries.at(entries.size() - 1).scan = utils::str_split(line, " = ").at(1);
            look_for_pattern = false;
        }
        else if (std::string _variable_type = utils::extract_variable_type(line); !look_for_pattern && _variable_type != "")
        {
            // std::cout << "push: " << line << " " << utils::str_contains(line, " = ") << std::endl;
            std::string a = utils::str_replace(line, utils::str_split(line, " = ").at(1), "_placeholder_;");
            entries.push_back({file_path, line_count, _variable_type, a});
            look_for_pattern = true;
        }

        line_count++;
    }

    file.close();
    return true;
}

std::string generate_buffer(const std::string& exe_file_path, std::vector<Entry>& entries)
{
    std::stringstream buffer;
    buffer << "#pragma once\n\n";
    buffer << "#include <cstdint>\n\n";
    buffer << "#include \"../deps/shared/src/Memory.h\"\n";
    buffer << "#include \"utils/string.h\"\n";
    buffer << "#include \"utils/io.h\"\n\n";

    buffer << "void run_static_replacement()\n{\n";
    buffer << std::format("\tstd::vector<uint8_t> bytes = utils::read_file(\"{}\");\n", exe_file_path);

    for (auto& offset : entries)
    {
        offset.scan = utils::str_replace(offset.scan, "*pat.scan()", "scan_bytes(bytes)");
        offset.scan = utils::str_replace(offset.scan, "get_offset", "get_offset2");
        offset.scan = utils::str_replace(offset.scan, std::format(".get<{}*>();", offset.type), ".get<uint32_t>()");

        buffer << "\n\tutils::overwrite_file_line(\n";
        buffer << "\t\t\"" << offset.file_path.c_str() << "\",\n";
        buffer << "\t\t" << offset.line << ",\n";
        buffer << "\t\tutils::str_replace(\"" << offset.out.c_str() << "\", ";
        buffer << "\"_placeholder_\", ";
        buffer << "std::to_string(utils::offset_from_bytes(bytes, Memory::Pattern(\"" << offset.pattern.c_str() << "\")";
        buffer << "." << offset.scan.c_str() << ")))\n\t);\n";
    }

    buffer << "}\n";

    return buffer.str();
}

bool generate_temp_file(const std::string& gen_file_path, const std::string& exe_file_path, const std::string& patterns_directory_path)
{
    std::vector<Entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(patterns_directory_path))
    {
        std::string file_path = entry.path().string();
        std::replace(file_path.begin(), file_path.end(), '\\', '/');

        if (!parse_entries(entries, file_path))
        {
            return false;
        }
    }

    utils::write_file(gen_file_path, generate_buffer(exe_file_path, entries));
    return true;
}
