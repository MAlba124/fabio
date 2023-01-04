#include <string>
#include <iostream>
#include <stdexcept>

#include "./include/config.hpp"
#include "./include/toml.hpp"

namespace config = server::config;

config::SConfig::SConfig(const std::string& fn)
{
    try
    {
        this->tbl = toml::parse_file(fn);
    }
    catch (const toml::parse_error& err)
    {
        throw std::invalid_argument("Failed to open configuration file");
    }

    std::cout << "Opened configuration file\n";
}

config::ConfigOptions
config::SConfig::parse() {
    return config::ConfigOptions { .serverName = "TestServer", .port = 130000, .address = "127.0.0.1" };
}

config::SConfig::~SConfig()
    = default;