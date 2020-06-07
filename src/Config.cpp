//
// Created by jakob on 26.11.19.
//

#include <spdlog/spdlog.h>
#include "../include/eos/Config.hpp"
#include "../include/eos/utils.hpp"

#define LOAD_CONFIG_OPTION(section, option, type)\
do {\
if (config[#section].HasMember(#option))\
if (config[#section][#option].Is##type())\
section.option = config[#section][#option].Get##type();\
else SPDLOG_WARN("Config option 'section.option' has wrong type, using default value");\
else SPDLOG_WARN("Config missing option 'section.option', using default value");\
} while (0)

eos::Config::Config(const std::string& path) : path(path) {
    std::string config_file;
    if (eos::utils::load_file(path, config_file)) {
        SPDLOG_INFO(fmt::format("Config file found at '{}'", path));
        config.Parse(config_file.c_str());
        if (!config.HasParseError()) {
            if (config.HasMember("window")) {
                LOAD_CONFIG_OPTION(window, title, String);
                LOAD_CONFIG_OPTION(window, width, Int);
                LOAD_CONFIG_OPTION(window, height, Int);
            } else
                SPDLOG_WARN("Config missing section window, using default values");

            if (config.HasMember("log")) {
                LOAD_CONFIG_OPTION(log, toFile, Bool);
                LOAD_CONFIG_OPTION(log, maxFiles, Int);
            } else
                SPDLOG_WARN("Config missing section log, using default values");

            if (config.HasMember("engine")) {
                LOAD_CONFIG_OPTION(engine, targetUps, Int);
                LOAD_CONFIG_OPTION(engine, targetFps, Int);
                LOAD_CONFIG_OPTION(engine, capFps, Bool);
            } else
                SPDLOG_WARN("Config missing section engine, using default values");

            SPDLOG_DEBUG("Config successfully loaded");
        } else {
            BOOST_LOG_TRIVIAL(warning)
                << fmt::format("Parsing config file failed: {}: {}, starting with default values",
                   config.GetErrorOffset(), config.GetParseError());
        }
    } else {
        SPDLOG_WARN("Config file not found, starting with default values");
    }
}