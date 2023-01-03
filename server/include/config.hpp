#ifndef __CONFIG_HPP
#define __CONFIG_HPP 1

#include <fstream>
#include <string>
#include "toml.hpp"

namespace server::config {
    class SConfig {
    private:
        toml::table tbl;
    public:
        explicit SConfig(const std::string& fn);
        ~SConfig();
    };
}

#endif //__CONFIG_HPP
