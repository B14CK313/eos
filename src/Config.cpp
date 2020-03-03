//
// Created by jakob on 26.11.19.
//

#include "../include/eos/Config.hpp"
#include <boost/format.hpp>
#include "../include/eos/utils.hpp"

eos::Config::Config(const std::string& path, const std::string& default_config) : path(path) {
    if(eos::utils::file_exists(path)) {
        try {
            BOOST_LOG_TRIVIAL(info) << boost::format("Config file found at '%s'") % path;
            config.readFile(path.c_str());
        } catch (const libconfig::FileIOException& fioex) {
            BOOST_LOG_TRIVIAL(error) << "Loading config file failed";
        } catch (const libconfig::ParseException& pex) {
            BOOST_LOG_TRIVIAL(error) << "Parsing config file failed";
        }
    } else {
        BOOST_LOG_TRIVIAL(info) << "Config file not found, starting with default values";
        config.readString(default_config);
    }
    BOOST_LOG_TRIVIAL(debug) << "Config successfully loaded";
}

void eos::Config::write() {
    try {
        config.writeFile(path.c_str());
        BOOST_LOG_TRIVIAL(info) << "Config saved";
    } catch (const libconfig::FileIOException& fioex){
        BOOST_LOG_TRIVIAL(error) << "Saving config file failed";
    }
}