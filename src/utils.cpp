//
// Created by jakob on 09.10.19.
//

#include "../include/eos/utils.hpp"
#include <spdlog/spdlog.h>
#include <sys/stat.h>

bool eos::utils::file_exists(const std::string& path) {
    struct stat buffer{};
    return (stat (path.c_str(), &buffer) == 0);
}

std::string eos::utils::load_file(const std::string& path) {
    // Use binary: https://utf8everywhere.org/#faq.crlf
    boost::nowide::ifstream ifstream{path.c_str(), std::ios::in | std::ios::binary};
    if (!ifstream) {
        SPDLOG_ERROR("Loading file {} failed. File does not exist.", path);
    }
    std::string content;
    ifstream.seekg(0, std::ios::end);
    content.resize(ifstream.tellg());
    ifstream.seekg(0, std::ios::beg);
    ifstream.read(&content[0], content.size());
    ifstream.close();
    return(content);
}