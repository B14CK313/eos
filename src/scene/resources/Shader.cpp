//
// Created by jakob on 19.06.20.
//

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include "eos/utils.hpp"
#include "eos/scene/resources/Shader.h"

eos::Shader::Shader(const std::string_view vertexPath, const std::string_view fragmentPath) {
    const std::string vertexShaderSourceString = eos::utils::load_file(vertexPath);
    const char * const vertexShaderSource = vertexShaderSourceString.c_str();

    const std::string fragmentShaderSourceString = eos::utils::load_file(fragmentPath);
    const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

    //compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertex);
    check_compile_errors(vertex, Type::VERTEX);
    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragment);
    check_compile_errors(fragment, Type::FRAGMENT);
    // shader Program
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);
    check_compile_errors(id_, Type::PROGRAM);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

eos::Shader::~Shader() {
    glDeleteProgram(id_);
}

void eos::Shader::use() const {
    glUseProgram(id_);
}

const eos::Shader& eos::Shader::set_bool(const std::string_view name, bool value) const {
    glUniform1i(get_uniform_location(name), (int) value);
    return *this;
}

const eos::Shader& eos::Shader::set_int(const std::string_view name, int value) const {
    glUniform1i(get_uniform_location(name), value);
    return *this;
}

const eos::Shader& eos::Shader::set_uint(const std::string_view name, unsigned int value) const {
    glUniform1ui(get_uniform_location(name), value);
    return *this;
}

const eos::Shader& eos::Shader::set_float(const std::string_view name, float value) const {
    glUniform1f(get_uniform_location(name), value);
    return *this;
}

const eos::Shader& eos::Shader::set_vec2(const std::string_view name, const glm::vec2& value) const {
    glUniform2fv(get_uniform_location(name), 1, &value[0]);
    return *this;
}

const eos::Shader& eos::Shader::set_vec2(const std::string_view name, float x, float y) const {
    glUniform2f(get_uniform_location(name), x, y);
    return *this;
}

const eos::Shader& eos::Shader::set_vec3(const std::string_view name, const glm::vec3& value) const {
    glUniform3fv(get_uniform_location(name), 1, &value[0]);
    return *this;
}

const eos::Shader& eos::Shader::set_vec3(const std::string_view name, float x, float y, float z) const {
    glUniform3f(get_uniform_location(name), x, y, z);
    return *this;
}

const eos::Shader& eos::Shader::set_vec4(const std::string_view name, const glm::vec4& value) const {
    glUniform4fv(get_uniform_location(name), 1, &value[0]);
    return *this;
}

const eos::Shader& eos::Shader::set_vec4(const std::string_view name, float x, float y, float z, float w) const {
    glUniform4f(get_uniform_location(name), x, y, z, w);
    return *this;
}

const eos::Shader& eos::Shader::set_mat2(const std::string_view name, const glm::mat2& mat) const {
    glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
    return *this;
}

const eos::Shader& eos::Shader::set_mat3(const std::string_view name, const glm::mat3& mat) const {
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
    return *this;
}

const eos::Shader& eos::Shader::set_mat4(const std::string_view name, const glm::mat4& mat) const {
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &mat[0][0]);
    return *this;
}

int eos::Shader::get_uniform_location(const std::string_view name) const {
    return glGetUniformLocation(id_, name.data());
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
