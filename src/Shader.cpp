//
// Created by jakob on 19.06.20.
//

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include "../include/eos/utils.hpp"
#include "../include/eos/Shader.h"

eos::Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    const std::string vertexShaderSourceString = eos::utils::load_file(vertexPath);
    const char * const vertexShaderSource = vertexShaderSourceString.c_str();

    const std::string fragmentShaderSourceString = eos::utils::load_file(fragmentPath);
    const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

    //compile shaders
    unsigned int vertex, fragment;
    // vertex shader_
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertex);
    check_compile_errors(vertex, Type::VERTEX);
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragment);
    check_compile_errors(fragment, Type::FRAGMENT);
    // shader_ Program
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);
    check_compile_errors(id_, Type::PROGRAM);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void eos::Shader::use() const {
    glUseProgram(id_);
}

const eos::Shader& eos::Shader::set_bool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int) value);
    return *this;
}

const eos::Shader& eos::Shader::set_int(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
    return *this;
}

const eos::Shader& eos::Shader::set_float(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
    return *this;
}

void eos::Shader::check_compile_errors(unsigned int shader, Type type) {
    int status;
    char infoLog[1024];
    if (type == Type::PROGRAM) {
        glGetProgramiv(shader, GL_LINK_STATUS, &status);
        if (status == 0) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            SPDLOG_ERROR("Error linking program: {}", infoLog);
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == 0) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            SPDLOG_ERROR("Error compiling {} shader: {}", type == Type::VERTEX ? "vertex" : "fragment", infoLog);
        }
    }
}
