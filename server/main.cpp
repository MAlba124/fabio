/**
 * @mainpage Poker Game Documentation
 *
 * @section Intro
 * Poker game
 */

#include <boost/log/trivial.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <cstdlib>

#include "./include/server.hpp"
#include "./include/config.hpp"
#include "./include/log.hpp"

#define DEFAULT_CONFIG_FILE_NAME "example_config.toml"

int
main(int argc, char **argv)
{
    std::string configFile;
    for (int i = 1; i < argc; i++)
    {
        if ((!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file"))
            && argc > i)
            configFile = argv[++i];
        else
        {
            std::cerr << "Invalid argument: "
                      << argv[i]
                      << std::endl;
            return EXIT_FAILURE;
        }
    }

    if (configFile.empty()) configFile = DEFAULT_CONFIG_FILE_NAME;

    server::config::SConfig conf(configFile);

    /* Exit if we could not open the confifuration file */
    if (!conf.success())
    {
        std::cerr << "Failed to open configuration file" << std::endl;
        return EXIT_FAILURE;
    }
    //else ?

    server::config::ConfigOptions options = conf.parse();

    servLog::init(options.logFile);
    BOOST_LOG_TRIVIAL(info) << "Initialized logger";

    try
    {
        server::Server s(options.port, options.address,
                         options.maxConcurrentGames, options.maxPlayersPerGame);
        BOOST_LOG_TRIVIAL(info) << "Attempting to start server";
        s.serve();
    }
    catch (const std::exception& err)
    {
        BOOST_LOG_TRIVIAL(fatal) << err.what();
    }

    return EXIT_SUCCESS;
}