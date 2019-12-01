//
// Created by jakob on 01.12.19.
//

#include "../include/eos/Config.hpp"

const std::string eos::Config::DEFAULT_CONFIG = R"#(
window:
{
    title: "EOS_GameEngine";
    size:
    {
        w = 800;
        h = 600;
    };
    pos:
    {
        x = 0;
        y = 0;
    };
};
logToFile = true;
maxLogFiles = 1;
targetUPS = 100;
targetFPS = 120;
capFPS = true;
)#";