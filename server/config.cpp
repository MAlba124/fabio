#include <string>
#include <iostream>
#include <stdexcept>

#include "./include/config.hpp"
#include "./include/toml.hpp"

server::config::SConfig::SConfig(const std::string& fn)
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

server::config::SConfig::~SConfig()
    = default;