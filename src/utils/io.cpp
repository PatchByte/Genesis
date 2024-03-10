#include "io.h"

namespace utils
{
    std::string path(const std::string& file_name)
    {
        std::string origin = "../../../../";
        return origin + file_name;
    }

    std::vector<uint8_t> read_file(const std::string& file_name)
    {
        std::ifstream file(file_name, std::ios::binary);
        if (!file.is_open())
        {
            printf("Failed to open the file at %s\n", file_name);
            return std::vector<uint8_t>{};
        }

        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return bytes;
    }

    bool overwrite_file_line(const std::string& file_name, int line_number, const std::string& new_content)
    {
        if (line_number > 0)
        {
            line_number--;
        }

        std::ifstream input_file(file_name);
        std::vector<std::string> lines;

        if (!input_file.is_open())
        {
            return false;
        }

        std::string line;
        while (std::getline(input_file, line))
        {
            lines.push_back(line);
        }

        input_file.close();
        if (line_number >= 0 && line_number < static_cast<int>(lines.size()))
        {
            lines[line_number] = new_content;
        }

        std::ofstream outputFile(file_name);
        if (!outputFile.is_open())
        {
            return false;
        }

        for (const auto& line : lines)
        {
            outputFile << line << std::endl;
        }

        outputFile.close();
        return true;
    }

    bool write_file(const std::string& file_name, const std::string& buffer)
    {
        std::ofstream out_file(file_name, std::ios::out | std::ios::app);
        if (!out_file.is_open())
        {
            return false;
        }

        out_file.write(buffer.c_str(), buffer.size());
        out_file.close();
        return true;
    }

    bool empty_file(const std::string& file_name)
    {
        std::ofstream file(file_name, std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            return false;
        }

        file.close();
        return true;
    }
} // namespace utils
