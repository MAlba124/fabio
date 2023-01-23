#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <utility>

#include "./include/server.hpp"
#include "./include/player.hpp"

namespace asio = boost::asio;

server::Server::Server(const int portn, const std::string& addr, int maxg,
                       int maxp)
    : ac(
        this->ioc,
        asio::ip::tcp::endpoint(
            asio::ip::address::from_string(addr),
            portn
        )
    ),
      games(maxg, maxp)
{
    /* "Prime" the ioc object with work */
    this->doAccept();
}

void
server::Server::doAccept()
{
    this->ac.async_accept(
        [this](boost::system::error_code ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                BOOST_LOG_TRIVIAL(info) << "New client conntected from: "
                                        << socket.remote_endpoint().address()
                                        << ":"
                                        << socket.remote_endpoint().port();

                std::make_shared<game::player::Player>
                (
                        this->getPIDCount(), games, "Default", 1000,
                        std::move(socket)
                )->start();
            }
            else
            {
                BOOST_LOG_TRIVIAL(error) << "Failed to accept: "
                                         << ec.what();
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

game::player::playerID server::Server::getPIDCount() {
    return this->pIDCount++;
}

server::Server::~Server()
    = default;
