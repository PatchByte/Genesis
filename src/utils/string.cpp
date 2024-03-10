#include "string.h"

#include <sstream>

namespace utils
{
    std::string extract_namespace(const std::string& line)
    {
        std::istringstream iss(line);
        std::string token;

        while (iss >> token)
        {
            if (token == "namespace")
            {
                if (iss >> token)
                {
                    return token;
                }
            }
        }

        return "";
    }

    std::string extract_variable_type(const std::string& line)
    {
        std::vector<std::string> tokens = str_split(line, " ");
        for (size_t i = 0, l = tokens.size(); i < l; ++i)
        {
            if (i + 2 > l)
            {
                break;
            }

            if (tokens[i + 2] == "=")
            {
                return tokens[i];
            }
        }

        return "";
    }

    std::string extract_str(const std::string& line)
    {
        size_t startPos = line.find("\"");
        if (startPos != std::string::npos)
        {
            size_t endPos = line.find("\"", startPos + 1);
            if (endPos != std::string::npos)
            {
                return line.substr(startPos + 1, endPos - startPos - 1);
            }
        }

        return "";
    }

    bool str_contains(const std::string& line, const std::string& word)
    {
        return line.find(word) != std::string::npos;
    }

    std::vector<std::string> str_split(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

    std::string str_replace(std::string str, const std::string& old_str, const std::string& new_str)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(old_str, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, old_str.length(), new_str);
            start_pos += new_str.length();
        }

        return str;
    }
} // namespace utils
