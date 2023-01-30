#include <boost/asio.hpp>
#include <iostream>
#include <exception>
#include <thread>
#include <cstdlib>

#include "../../net/client/include/client.hpp"

namespace asio = boost::asio;

int
main(void)
{
    try
    {
        asio::io_context ioContext;
        std::thread ioContextThread;
        net::client::Client client(ioContext);
        client.connect("127.0.0.1", 1233);

        if (!client.isConnected())
        {
            std::cerr << "Failed to connect\n";
            return EXIT_FAILURE;
        }

        ioContextThread = std::thread([&ioContext] { ioContext.run(); });

        client.sendPing();

        client.userRegister("SomeUsername", "asjhdkajshdjahsjhd");

        if (ioContextThread.joinable())
            ioContextThread.join();

        client.close();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " <<  e.what() << std::endl;
    }

    return 0;
}