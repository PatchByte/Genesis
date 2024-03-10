#pragma once

#include "stdafx.h"

struct Entry
{
    std::string file_path;
    uint16_t line;

    std::string type;
    std::string out;
    std::string pattern;
    std::string scan;
};

bool generate_temp_file(const std::string& gen_file_path, const std::string& exe_file_path, const std::string& patterns_directory_path);
