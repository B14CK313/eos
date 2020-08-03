//
// Created by jakob on 09.10.19.
//

#ifndef EOS_UTILS_HPP
#define EOS_UTILS_HPP

#include <string>
#include <vector>
#include <glad/glad.h>
#include <fstream>
#include <spdlog/spdlog.h>

namespace eos::utils {
    //TODO: Split into files (fileaccess, ...)

    bool file_exists(const std::string& path);

    std::string load_file(const std::string& path);

    template <typename T>
    bool load_file(const std::string& path, T& content) {
        std::ifstream ifstream{path, std::ios::in | std::ios::binary};
        if (!ifstream) {
            SPDLOG_ERROR("Loading file {} failed. File does not exist.", path);
            return false;
        }
        ifstream.seekg(0, std::ios::end);
        content.resize(ifstream.tellg());
        ifstream.seekg(0, std::ios::beg);
        ifstream.read(&content[0], content.size());
        ifstream.close();
        return true;
    }

    std::vector<unsigned char> load_file_unsigned_char(const std::string& path);
}

#endif //EOS_UTILS_HPP
