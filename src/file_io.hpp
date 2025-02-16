#pragma once

#include "utils.hpp"

namespace FileIO {

    std::expected <std::vector<char>, std::error_code> readBinaryFile(const std::string& fileName);
    std::expected <std::string, std::error_code> readTextFile(const std::string& fileName);

    std::expected<void, std::error_code> writeTextFile(const std::string& fileName, const std::string& content, bool append = false);
    std::expected<void, std::error_code> writeBinaryFile(const std::string& fileName, const std::vector<char>& content, bool append = false);
};