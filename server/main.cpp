#include <boost/asio.hpp>
#include <cstdlib>
#include <exception>
#include <iostream>

#include "./include/server.hpp"

int
main(int argc, char **argv)
{
    int port;
    if (argc < 2)
    {
        std::cerr << argv[0] << " <port>\n";
        return EXIT_FAILURE;
    }

    port = atoi(argv[1]);

    try
    {
        server::Server s(port);
        s.serve();
    }
    catch (std::exception& err)
    {
        std::cerr << err.what() << std::endl;
    }

    return 0;
}