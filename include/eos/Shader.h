//
// Created by jakob on 19.06.20.
//

#ifndef GAME_SHADER_H
#define GAME_SHADER_H

#include <string>

namespace eos {

    class Shader {
    public:
        enum class Type {PROGRAM, FRAGMENT, VERTEX};

        // the program ID
        unsigned int ID;

        // constructor reads and builds the shader
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        // use/activate the shader
        void use() const;

        // utility uniform functions
        void setBool(const std::string& name, bool value) const;

        void setInt(const std::string& name, int value) const;

        void setFloat(const std::string& name, float value) const;

    private:
        static void checkCompileErrors(unsigned int shader, const Type type);
    };

}

#endif //GAME_SHADER_H
