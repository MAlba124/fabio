#ifndef _SERVER_HPP
#define _SERVER_HPP 1

#include <boost/asio.hpp>
#include <string>

/* Forward declarations to eliminate "'x' was has not been declared" error */
namespace server
{
    class Server;
}

#include "./games.hpp"
#include "./player.hpp"

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
        //std::vector<game::Game> games;
        Games games;

        /**
         * @brief The maximum amount of games that the server can
         * run concurrently
         */
        //int maxGames;
        game::player::playerID pIDCount = 0;
    public:
        /**
         * @brief Constructor
         * @param portn Port to listen for connections on
         * @param addr Address to listen to
         * @param maxg Maximum amount of concurrent games running at any
         * given time
         */
        explicit Server(int portn, const std::string& addr, int maxg, int maxp);

        /**
         * @brief Start listening and accepting clients
         */
        void serve();

        game::player::playerID getPIDCount();

        /**
         * @brief Deconstructor
         */
        ~Server();
    private:
        /**
         * @brief Accept function called after handling accepts
         */
        void doAccept();
    };
}

#endif //_SERVER_HPP
