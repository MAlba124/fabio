#include <string>
#include <stdexcept>

#include "./include/config.hpp"
#include "./include/toml.hpp"

/* Default values that will be used if they were not provided in
 * a configuration file */
/* [Server] */
static const int DEFAULT_PORT          = 3000;
static const char* DEFAULT_ADDRESS     = "127.0.0.1";
static const char* DEFAULT_SERVER_NAME = "Poker";

/* [Logging */
static const char* DEFAULT_LOGFILE = "poker_server_log.txt";

/* [Game] */
static const int DEFAULT_MC_GAMES      = 10;
static const int DEFAULT_MP_GAME       = 6;

namespace config = server::config;

config::SConfig::SConfig(const std::string& fn)
    : name(fn)
{
    try
    {
        this->tbl = toml::parse_file(fn);
    }
    catch (const toml::parse_error& err)
    {
        this->err = false;
        return;
    }
    this->err = true;
}

config::ConfigOptions
config::SConfig::parse() {
    return config::ConfigOptions {
        // ----------------------↓
        .serverName = this->tbl["Server"]["serverName"]
                .value_or(DEFAULT_SERVER_NAME),
        .port = this->tbl["Server"]["port"].value_or(DEFAULT_PORT),
        .address = this->tbl["Server"]["address"].value_or(DEFAULT_ADDRESS),

        // ----------------------↓
        .logFile = this->tbl["Logging"]["output"].value_or(DEFAULT_LOGFILE),

        // -----------------------------↓
        .maxConcurrentGames = this->tbl["Game"]["maxConcurrentGames"]
                .value_or(DEFAULT_MC_GAMES),
        .maxPlayersPerGame = this->tbl["Game"]["maxPlayersPerGame"]
                .value_or(DEFAULT_MP_GAME)
    };
}

std::string
config::SConfig::getName()
{
    return this->name;
}

bool
config::SConfig::success() const
{
    return this->err;
}

config::SConfig::~SConfig()
    = default;