#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>

#include "game.hpp"

namespace asio = boost::asio;

/**
 * @brief Class that handles the server
 */
namespace server
{
    class Server {
    private:
        /**
         * @brief io_context object to do the actual networking I/O
         */
        asio::io_context ioc;

        /**
         * @brief acceptor object
         */
        asio::ip::tcp::acceptor ac;

        /**
         * @brief Containing all the running/created games
         */
        std::vector<game::Game> games;

        /**
         * @brief The maximum amount of games that the server can
         * run concurrently
         */
        int maxGames;
    public:
        /**
         * @brief Constructor
         * @param portn Port to listen for connections on
         * @param addr Address to listen to
         * @param maxg Maximum amount of concurrent games running at any
         * given time
         */
        explicit Server(int portn, const std::string& addr, int maxg);

        /**
         * @brief Start listening and accepting clients
         */
        void serve();

        /**
         * @brief Deconstructor
         */
        ~Server();
    private:
        /**
         * @param mp Maximum amount of players the game can have
         * @param owner The nickname of the player that owns the game
         * @return Returns true if the game was created
         */
        bool createNewGame(int mp, const std::string& owner);

        /**
         * @brief Accept function called after handling accepts
         */
        void doAccept();
    };
}

#endif //_SERVER_HPP
