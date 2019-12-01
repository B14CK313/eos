//
// Created by jakob on 26.11.19.
//

#include "Config.hpp"
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include "utils.hpp"

const char* eos::Config::DEFAULT_CONFIG = R"#(
game:
{
    window:
    {
        title: "EOS_GameEngine Test";
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
};
eos:
{
    targetUPS = 100;
    targetFPS = 120;
    capFPS = true;
};)#";

eos::Config::Config(const std::string& path) : path(path) {
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
        config.readString(DEFAULT_CONFIG);
    }
    BOOST_LOG_TRIVIAL(debug) << "Config successfully loaded";
}

void eos::Config::write() {
    try {
        config.writeFile(path.c_str());
    } catch (const libconfig::FileIOException& fioex){
        BOOST_LOG_TRIVIAL(error) << "Saving config file failed";
    }
}

template <typename T>
T eos::Config::get(const std::string& key) {
    T value;
    if(!config.lookupValue(key, value))
        BOOST_LOG_TRIVIAL(warning) << boost::format("Setting '%s' not found") % key;
    return value;
}
template bool eos::Config::get<bool>(const std::string& key);
template int eos::Config::get<int>(const std::string& key);
template std::string eos::Config::get<std::string>(const std::string& key);

template <typename T>
void eos::Config::set(const std::string& key, T value){

}
template void eos::Config::set<bool>(const std::string& key, bool value);
template void eos::Config::set<int>(const std::string& key, int value);
template void eos::Config::set<std::string>(const std::string& key, std::string value);
