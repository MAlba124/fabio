#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>

#include "game.hpp"

namespace asio = boost::asio;

namespace server
{
    class Server {
    private:
        asio::io_context ioc;
        asio::ip::tcp::acceptor ac;
        std::vector<game::Game> games;
        int maxGames;
    public:
        explicit Server(int portn, const std::string& addr, int maxg);
        void serve();
        ~Server();
    private:
        int createNewGame(int mp, std::string owner);
        void doAccept();
    };
}

#endif //_SERVER_HPP
