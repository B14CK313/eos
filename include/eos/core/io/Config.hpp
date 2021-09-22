//
// Created by jakob on 26.11.19.
//

#pragma once

#include <string>
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
            int maxFiles{3};
            std::string fileName{"log.log"};
        } log;

        struct Engine {
            int targetUps{100};
            int targetFps{120};
            bool capFps{true};
            bool vsync{false};
        } engine;

    private:
        std::string path_;
        rapidjson::Document config_;
    };

}
