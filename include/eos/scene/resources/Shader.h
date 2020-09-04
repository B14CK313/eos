//
// Created by jakob on 19.06.20.
//

#ifndef EOS_SHADER_H
#define EOS_SHADER_H

#include <string>
#include <glm/glm.hpp>

namespace eos {

    class Shader {
    public:
        enum struct Type {PROGRAM, FRAGMENT, VERTEX};

        // constructor reads and builds the shader
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        ~Shader();

        // use/activate the shader
        void use() const;

        // utility uniform functions
        const Shader& set_bool_uniform(const std::string& name, bool value) const;

        const Shader& set_int_uniform(const std::string& name, int value) const;

        const Shader& set_uint_uniform(const std::string& name, unsigned int value) const;

        const Shader& set_float_uniform(const std::string& name, float value) const;

        const Shader& set_vec2_uniform(const std::string& name, const glm::vec2& value) const;

        const Shader& set_vec2_uniform(const std::string& name, float x, float y) const;

        const Shader& set_vec3_uniform(const std::string& name, const glm::vec3& value) const;

        const Shader& set_vec3_uniform(const std::string& name, float x, float y, float z) const;

        const Shader& set_vec4_uniform(const std::string& name, const glm::vec4& value) const;

        const Shader& set_vec4_uniform(const std::string& name, float x, float y, float z, float w) const;

        const Shader& set_mat2_uniform(const std::string& name, const glm::mat2& mat) const;

        const Shader& set_mat3_uniform(const std::string& name, const glm::mat3& mat) const;

        const Shader& set_mat4_uniform(const std::string& name, const glm::mat4& mat) const;

        int get_uniform_location(const std::string& name) const;

        int get_attribute_location(const std::string& name) const;

    private:
        // the program ID
        unsigned int id_;

        static void check_compile_errors(unsigned int shader, Type type);
    };

}

#endif //EOS_SHADER_H
