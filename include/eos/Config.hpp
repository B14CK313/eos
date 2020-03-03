//
// Created by jakob on 26.11.19.
//

#ifndef GLFW_GAMEENGINE_CONFIG_HPP
#define GLFW_GAMEENGINE_CONFIG_HPP

#include <string>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <libconfig.h++>

namespace eos {

    class Config {
    public:
        static const std::string DEFAULT_CONFIG;

        Config(const std::string& path, const std::string& default_config);

        template <typename T>
        T get(const std::string& key) {
            T setting;
            if(!config.lookupValue(key, setting))
                BOOST_LOG_TRIVIAL(warning) << boost::format("Setting '%s' not found") % key;
            return setting;
        }

        // TODO: Overload instead of template -> able to add options
        template <typename T>
        void set(const std::string& key, T value){
            if(config.exists(key)) {
                auto& setting = config.lookup(key);
                setting = value;
            } else {
                BOOST_LOG_TRIVIAL(warning) << boost::format("Setting '%s' not found") % key;
            }
        }

        void write();
    private:
        std::string path;
        libconfig::Config config;
    };

}

#endif //GLFW_GAMEENGINE_CONFIG_HPP
