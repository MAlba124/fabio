#ifndef __CONFIG_HPP
#define __CONFIG_HPP 1

#include <fstream>
#include <string>
#include <vector>

#include "toml.hpp"
#include "game.hpp"


namespace server::config {
    struct ConfigOptions {
        std::string serverName;
        int port;
        std::string address;
        int maxConcurrentGames;
        int maxPlayersPerGame;
    };

    class SConfig {
    private:
        toml::table tbl;
        std::vector<game::Game> games;
    public:
        explicit SConfig(const std::string& fn);
        ~SConfig();
        ConfigOptions parse();
    };
}

#endif //__CONFIG_HPP
