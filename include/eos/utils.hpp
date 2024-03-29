//
// Created by jakob on 09.10.19.
//

#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <fstream>
#include <spdlog/spdlog.h>
#include <boost/nowide/fstream.hpp>

namespace eos {
    namespace utils {
        //TODO: Split into files (fileaccess, ...)

        bool file_exists(const std::string& path);

        std::string load_file(const std::string& path);

        template<typename T>
        bool load_file(const std::string& path, T& content) {
            // Use binary: https://utf8everywhere.org/#faq.crlf
            boost::nowide::ifstream ifstream{path.c_str(), std::ios::in | std::ios::binary};
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
    }
}
