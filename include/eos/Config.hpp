//
// Created by jakob on 26.11.19.
//

#ifndef GLFW_GAMEENGINE_CONFIG_HPP
#define GLFW_GAMEENGINE_CONFIG_HPP

#include <string>
#include <libconfig.h++>

namespace eos {

    class Config {
    public:
        explicit Config(const std::string& path);

        template<typename T>
        T get(const std::string& key);

        template<typename T>
        void set(const std::string& key, T value);

        void write();

    private:
        static const char* DEFAULT_CONFIG;
        std::string path;
        libconfig::Config config;
    };

}

#endif //GLFW_GAMEENGINE_CONFIG_HPP
