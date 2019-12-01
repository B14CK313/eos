//
// Created by jakob on 09.10.19.
//

#include "../include/eos/utils.hpp"
#include <sys/stat.h>
#include <boost/log/trivial.hpp>

bool eos::utils::file_exists(const std::string &path) {
    struct stat buffer{};
    return (stat (path.c_str(), &buffer) == 0);
}
