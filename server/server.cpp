#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>
#include <utility>

#include "./include/server.hpp"
#include "./include/game.hpp"

namespace asio = boost::asio;

server::Server::Server(const int portn, const std::string& addr, int maxg)
    : ac(
        this->ioc,
        asio::ip::tcp::endpoint(
            asio::ip::address::from_string(addr),
            portn
        )
    ),
      maxGames(maxg)
{
    /* "Prime" the ioc object with work */
    this->doAccept();

    /* Create a game for testing purposes */
    this->createNewGame(2, "Admin");
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
                        "Default", 1000, std::move(socket)
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

//int server::Server::createNewGame(int mp, std::string owner) {
bool
server::Server::createNewGame(int, const std::string&)
{
    if ((int)this->games.size() >= this->maxGames)
        return false;

    //this->games.emplace_back(mp, std::move(owner));

    return true;
}

server::Server::~Server()
    = default;
