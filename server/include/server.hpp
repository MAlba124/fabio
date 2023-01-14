#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>

#include "game.hpp"

namespace asio = boost::asio;

/**
 * Class that handles the server
 */
namespace server
{
    class Server {
    private:
        asio::io_context ioc;
        asio::ip::tcp::acceptor ac;
        std::vector<game::Game> games;
        int maxGames;
    public:
        /**
         * @param portn Port to listen for connections on
         * @param addr Address to listen to
         * @param maxg Maximum amount of concurrent games running at any given time
         */
        explicit Server(int portn, const std::string& addr, int maxg);

        /**
         * Start listening and accepting clients
         */
        void serve();

        ~Server();
    private:
        /**
         * @param mp Maximum amount of players the game can have
         * @param owner The nickname of the player that owns the game
         * @return Returns true if the game was created
         */
        bool createNewGame(int mp, const std::string& owner);

        void doAccept();
    };
}

#endif //_SERVER_HPP
