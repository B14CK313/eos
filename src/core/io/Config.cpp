//
// Created by jakob on 26.11.19.
//

#include <spdlog/spdlog.h>
#include "eos/core/io/Config.hpp"
#include "eos/utils.hpp"

#define LOAD_CONFIG_OPTION(section, option, type)\
do {\
if (config_[#section].HasMember(#option))\
if (config_[#section][#option].Is##type())\
section.option = config_[#section][#option].Get##type();\
else SPDLOG_WARN("Config option 'section.option' has wrong type, using default value");\
else SPDLOG_WARN("Config missing option 'section.option', using default value");\
} while (0)

eos::Config::Config(const std::string_view path) : path_(path) {
    std::string configFile;
    if (eos::utils::load_file(path, configFile)) {
        SPDLOG_INFO(fmt::format("Config file found at '{}'", path));
        config_.Parse(configFile.c_str());
        if (!config_.HasParseError()) {
            if (config_.HasMember("window")) {
                LOAD_CONFIG_OPTION(window, title, String);
                LOAD_CONFIG_OPTION(window, width, Int);
                LOAD_CONFIG_OPTION(window, height, Int);
            } else
                SPDLOG_WARN("Config missing section window, using default values");

            if (config_.HasMember("log")) {
                LOAD_CONFIG_OPTION(log, maxFiles, Int);
                LOAD_CONFIG_OPTION(log, fileName, String);
            } else
                SPDLOG_WARN("Config missing section log, using default values");

            if (config_.HasMember("engine")) {
                LOAD_CONFIG_OPTION(engine, targetUps, Int);
                LOAD_CONFIG_OPTION(engine, targetFps, Int);
                LOAD_CONFIG_OPTION(engine, capFps, Bool);
            } else
                SPDLOG_WARN("Config missing section engine, using default values");

            SPDLOG_DEBUG("Config successfully loaded");
        } else {
            SPDLOG_WARN("Parsing config file failed: {}: {}, starting with default values", config_.GetErrorOffset(), config_.GetParseError());
        }
    } else {
        SPDLOG_WARN("Config file not found, starting with default values");
    }
}