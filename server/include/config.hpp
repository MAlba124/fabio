#ifndef __CONFIG_HPP
#define __CONFIG_HPP 1

#include <fstream>
#include <string>
#include "toml.hpp"


namespace server::config {
    struct ConfigOptions {
        std::string serverName;
        //std::vector<int> ports;
        int port;
        std::string address;
    };

    class SConfig {
    private:
        toml::table tbl;
    public:
        explicit SConfig(const std::string& fn);
        ~SConfig();
        ConfigOptions parse();
    };
}

#endif //__CONFIG_HPP
