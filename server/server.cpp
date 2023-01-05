#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "./include/server.hpp"
#include "./include/game.hpp"

namespace asio = boost::asio;

server::Server::Server(const int portn, const std::string& addr)
    : ac(this->ioc, asio::ip::tcp::endpoint(asio::ip::address::from_string(addr),
                                            portn))
{
    /* "Prime" the ioc object with work */
    this->doAccept();

    /* Create a game for testing purposes */
    this->games.emplace_back(2, "Admin");
}

void
server::Server::doAccept()
{
    this->ac.async_accept(
        [this](boost::system::error_code ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                std::cout << "New client connected from: "
                          << socket.remote_endpoint().address().to_string()
                          << std::endl;
                games.back().join(socket);
            }
            else
            {
                std::cerr << ec.what() << std::endl;
                socket.close();
            }

            this->doAccept();
        }
    );
}

/*
 * Run the private io_context object, aka make the server able to
 * accept, send and receive
 */
void
server::Server::serve()
{
    this->ioc.run();
}

server::Server::~Server()
    = default;
