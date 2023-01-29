#ifndef __CONFIG_HPP
#define __CONFIG_HPP 1

#include <fstream>
#include <string>
#include <vector>

#include "toml.hpp"
#include "game.hpp"


namespace server::config {

    /**
     * @brief Structure containing all available options
     */
    struct ConfigOptions {
        std::string serverName;
        int port;
        std::string address;

        std::string logFile;

        std::string userDatabase;

        int maxConcurrentGames;
        int maxPlayersPerGame;
    };

    /**
     * @brief Server config class
     */
    class SConfig {
    private:
        /**
         * @brief toml++ table object
         */
        toml::table tbl;

        //std::vector<game::Game> games;
        //
        /**
         * @brief The name of the configuration file
         */
        std::string name;

        /**
         * @brief Value indicating if an error occurred when constructing
         */
        bool err;
    public:
        /**
         * @brief Constructor
         * @param fn Name of the configuration file
         */
        explicit SConfig(const std::string& fn);

        /**
         * @brief Deconstructor
         */
        ~SConfig();

        /**
         * @brief Parse the configuration file
         * @return ConfigOptions struct
         */
        ConfigOptions parse();

        /**
         * @return The name of the configuration file
         */
        std::string getName();

        /**
         * @return The value of err
         */
        [[nodiscard]] bool success() const;
    };
}

#endif //__CONFIG_HPP
