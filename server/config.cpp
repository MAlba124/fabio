#include <string>
#include <iostream>
#include <stdexcept>

#include "./include/config.hpp"
#include "./include/toml.hpp"

/* Default values that will be used if they were not provided in
 * a configuration file */
static const int DEFAULT_PORT          = 3000;
static const char* DEFAULT_ADDRESS     = "127.0.0.1";
static const char* DEFAULT_SERVER_NAME = "Poker";

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
    return config::ConfigOptions {
        .serverName = this->tbl["Server"]["serverName"].value_or(DEFAULT_SERVER_NAME),
        .port = this->tbl["Server"]["port"].value_or(DEFAULT_PORT),
        .address = this->tbl["Server"]["address"].value_or(DEFAULT_ADDRESS)
    };
}

config::SConfig::~SConfig()
    = default;