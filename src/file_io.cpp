#include "file_io.hpp"

#include <fstream>

namespace FileIO {

    std::expected <std::vector<char>, std::error_code> readBinaryFile(const std::string& fileName) {

        std::filesystem::path filePath(fileName);
        if(!std::filesystem::exists(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        if(std::filesystem::is_directory(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::is_a_directory));
        }

        if(!std::filesystem::is_regular_file(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
        }

        std::ifstream file(fileName, std::ios::in | std::ios::binary);

        if(!file.is_open()) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }

        file.seekg(0, file.end);
        std::streampos length = file.tellg();
        file.seekg(0, file.beg);

        std::vector<char> content;
        content.resize(length);

        file.read(content.data(), length);

        if(file.fail()) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }
        return content;
    }

    std::expected<std::string, std::error_code> readTextFile(const std::string& fileName) {

        std::filesystem::path filePath(fileName);
        if(!std::filesystem::exists(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        if(std::filesystem::is_directory(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::is_a_directory));
        }

        if(!std::filesystem::is_regular_file(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
        }

        std::ifstream file(fileName, std::ios::in);

        if(!file.is_open()) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }

        std::string content, line;

        while(std::getline(file, line)) {
            content += line;
            if(!file.eof()) {
                content += '\n';
            }
        }

        return content;
    }

    std::expected<void, std::error_code> writeTextFile(const std::string& fileName, const std::string& content, bool append) {

        std::filesystem::path filePath(fileName);

        if(!std::filesystem::exists(filePath.parent_path())) {
            return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        if(std::filesystem::is_directory(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::is_a_directory));
        }

        std::ios::openmode writeMode = (append == true) ? std::ios::app : std::ios::trunc;
        std::ofstream file(filePath, std::ios::out | writeMode);
        if(!file.is_open()) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }

        file << content << std::endl;

        return {};
    }
    std::expected<void, std::error_code> writeBinaryFile(const std::string& fileName, const std::vector<char>& content, bool append) {

        std::filesystem::path filePath(fileName);

        if(!std::filesystem::exists(filePath.parent_path())) {
            return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        if(std::filesystem::is_directory(filePath)) {
            return std::unexpected(std::make_error_code(std::errc::is_a_directory));
        }

        std::ios::openmode writeMode = (append == true) ? std::ios::app : std::ios::trunc;
        std::ofstream file(filePath, std::ios::out | std::ios::binary | writeMode);
        if(!file.is_open()) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }

        file.write(content.data(), content.size());

        return {};
    }
}