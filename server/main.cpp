#include <exception>
#include <iostream>
#include <string>

#include "./include/server.hpp"
#include "./include/config.hpp"

#define DEFAULT_CONFIG_FILE_NAME "example_config.toml"

int
main(int argc, char **argv)
{
    std::string configFile;
    for (int i = 0; i < argc; i++)
    {
        if ((!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config-file")) && argc > i)
            configFile = argv[++i];
    }

    if (configFile.empty()) configFile = DEFAULT_CONFIG_FILE_NAME;

    try
    {
        server::config::SConfig conf(configFile);
        server::config::ConfigOptions options = conf.parse();

        server::Server s(options.port, options.address, options.maxConcurrentGames);
        s.serve();
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
    }

    return 0;
}