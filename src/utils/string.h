#pragma once

#include "../stdafx.h"
#include <assert.h>

namespace utils
{
    template <class T>
    inline T offset_from_bytes(const std::vector<uint8_t>& bytes, T index = 0)
    {
        assert(sizeof(T) <= (bytes.size() - index));
        return *((T*)(bytes.data() + index));
    }

    std::string extract_namespace(const std::string& line);
    std::string extract_variable_type(const std::string& line);
    std::string extract_str(const std::string& line);

    bool str_contains(const std::string& line, const std::string& word);
    std::vector<std::string> str_split(std::string s, std::string delimiter);
    std::string str_replace(std::string str, const std::string& old_str, const std::string& new_str);
} // namespace utils
