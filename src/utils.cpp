//
// Created by jakob on 09.10.19.
//

#include "../include/eos/utils.hpp"
#include <sys/stat.h>
#include <boost/log/trivial.hpp>
#include <fmt/core.h>

bool eos::utils::file_exists(const std::string &path) {
    struct stat buffer{};
    return (stat (path.c_str(), &buffer) == 0);
}

std::string eos::utils::load_file(const std::string& path) {
    std::ifstream ifstream{path, std::ios::in | std::ios::binary};
    if (!ifstream) {
        BOOST_LOG_TRIVIAL(error) << fmt::format("Loading file {} failed. File does not exist.", path);
    }
    std::string content;
    ifstream.seekg(0, std::ios::end);
    content.resize(ifstream.tellg());
    ifstream.seekg(0, std::ios::beg);
    ifstream.read(&content[0], content.size());
    ifstream.close();
    return(content);
}

bool eos::utils::load_file(const std::string& path, std::string& content) {
    std::ifstream ifstream{path, std::ios::in | std::ios::binary};
    if (!ifstream) {
        BOOST_LOG_TRIVIAL(error) << fmt::format("Loading file {} failed. File does not exist.", path);
        return false;
    }
    ifstream.seekg(0, std::ios::end);
    content.resize(ifstream.tellg());
    ifstream.seekg(0, std::ios::beg);
    ifstream.read(&content[0], content.size());
    ifstream.close();
    return true;
}
