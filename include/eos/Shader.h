//
// Created by jakob on 19.06.20.
//

#ifndef EOS_SHADER_H
#define EOS_SHADER_H

#include <string>

namespace eos {

    class Shader {
    public:
        enum class Type {PROGRAM, FRAGMENT, VERTEX};

        // the program ID
        unsigned int id;

        // constructor reads and builds the shader
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        // use/activate the shader
        void use() const;

        // utility uniform functions
        void set_bool(const std::string& name, bool value) const;

        void set_int(const std::string& name, int value) const;

        void set_float(const std::string& name, float value) const;

    private:
        static void check_compile_errors(unsigned int shader, Type type);
    };

}

#endif //EOS_SHADER_H
