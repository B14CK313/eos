//
// Created by jakob on 26.11.19.
//

#include "../include/eos/Config.hpp"
#include <fmt/core.h>
#include "../include/eos/utils.hpp"

#define LOAD_CONFIG_OPTION(section, option, type)\
do {\
if (config[#section].HasMember(#option))\
if (config[#section][#option].Is##type())\
section.option = config[#section][#option].Get##type();\
else BOOST_LOG_TRIVIAL(warning) << "Config option 'section.option' has wrong type, using default value";\
else BOOST_LOG_TRIVIAL(warning) << "Config missing option 'section.option', using default value";\
} while (0)

eos::Config::Config(const std::string& path) : path(path) {
    std::string config_file;
    if (eos::utils::load_file(path, config_file)) {
        BOOST_LOG_TRIVIAL(info) << fmt::format("Config file found at '{}'", path);
        config.Parse(config_file.c_str());
        if (!config.HasParseError()) {
            if (config.HasMember("window")) {
                LOAD_CONFIG_OPTION(window, title, String);
                LOAD_CONFIG_OPTION(window, width, Int);
                LOAD_CONFIG_OPTION(window, height, Int);
            } else
                BOOST_LOG_TRIVIAL(warning) << "Config missing section window, using default values";

            if (config.HasMember("log")) {
                LOAD_CONFIG_OPTION(log, toFile, Bool);
                LOAD_CONFIG_OPTION(log, maxFiles, Int);
            } else
                BOOST_LOG_TRIVIAL(warning) << "Config missing section log, using default values";

            if (config.HasMember("engine")) {
                LOAD_CONFIG_OPTION(engine, targetUps, Int);
                LOAD_CONFIG_OPTION(engine, targetFps, Int);
                LOAD_CONFIG_OPTION(engine, capFps, Bool);
            } else
                BOOST_LOG_TRIVIAL(warning) << "Config missing section engine, using default values";

            BOOST_LOG_TRIVIAL(debug) << "Config successfully loaded";
        } else {
            BOOST_LOG_TRIVIAL(warning)
                << fmt::format("Parsing config file failed: {}: {}, starting with default values",
                   config.GetErrorOffset(), config.GetParseError());
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Config file not found, starting with default values";
    }
}