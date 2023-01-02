#include <boost/asio.hpp>
#include <iostream>

#include "./include/server.hpp"

namespace asio = boost::asio;

server::Server::Server(int portn)
    : ac(this->ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), portn))
{
    /* "Prime" the ioc object with work */
    this->doAccept();
}

void
server::Server::doAccept(void)
{
    this->ac.async_accept(
        [this](boost::system::error_code ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                std::cout << "New client connected from: "
                          << socket.remote_endpoint().address().to_string()
                          << std::endl;
                socket.close();
            }
            else
            {
                std::cerr << ec.what() << std::endl;
            }

            this->doAccept();
        }
    );
}

/*
 * Run the private io_context object, aka make the server able to
 * accept, send/recive
 */
void
server::Server::serve(void)
{
    this->ioc.run();
}

server::Server::~Server(void)
{
}
