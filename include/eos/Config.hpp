//
// Created by jakob on 26.11.19.
//

#ifndef GLFW_GAMEENGINE_CONFIG_HPP
#define GLFW_GAMEENGINE_CONFIG_HPP

#include <string>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <rapidjson/document.h>

namespace eos {

    class Config {
    public:
        explicit Config(const std::string& path);

        struct Window {
            std::string title{"EOS GameEngine"};
            int width{800};
            int height{600};
        } window;

        struct Log {
            bool toFile{true};
            int maxFiles{3};
        } log;

        struct Engine {
            int targetUps{100};
            int targetFps{120};
            bool capFps{true};
        } engine;

    private:
        std::string path;
        rapidjson::Document config;
    };

}

#endif //GLFW_GAMEENGINE_CONFIG_HPP
