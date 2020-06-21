//
// Created by jakob on 09.10.19.
//

#ifndef GLFW_GAMEENGINE_UTILS_HPP
#define GLFW_GAMEENGINE_UTILS_HPP

#include <string>
#include <vector>
#include <glad/glad.h>
#include <fstream>

namespace eos::utils {
    bool file_exists(const std::string& path);

    std::string load_file(const std::string& path);

    bool load_file(const std::string& path, std::string& content);
}

#endif //GLFW_GAMEENGINE_UTILS_HPP
