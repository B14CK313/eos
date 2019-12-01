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
        static const std::string DEFAULT_CONFIG;

        Config(const std::string& path, const std::string& default_config);

        template<typename T>
        T get(const std::string& key);

        template<typename T>
        void set(const std::string& key, T value);

        void write();
    private:
        std::string path;
        libconfig::Config config;
    };

}

#endif //GLFW_GAMEENGINE_CONFIG_HPP
