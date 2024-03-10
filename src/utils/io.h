#pragma once

#include "../stdafx.h"

namespace utils
{
    std::string path(const std::string& file_name);
    std::vector<uint8_t> read_file(const std::string& file_name);

    bool overwrite_file_line(const std::string& file_name, int line_number, const std::string& new_content);
    bool write_file(const std::string& file_name, const std::string& buffer);
    bool empty_file(const std::string& file_name);
} // namespace utils
