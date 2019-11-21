//
// Created by jakob on 09.10.19.
//

#ifndef GLFW_GAMEENGINE_UTILS_HPP
#define GLFW_GAMEENGINE_UTILS_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

namespace eos::util {
    GLuint load_shader(GLenum type, std::string path);

    GLuint create_program(std::vector<GLuint> shaderList);
}

#endif //GLFW_GAMEENGINE_UTILS_HPP
