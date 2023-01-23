/* TODO: Write doc */

#ifndef _PLAYER_HPP
#define _PLAYER_HPP 1

#include <boost/asio.hpp>
#include <string>
#include <memory>
#include <deque>

/* Forward declarations to eliminate "'x' was has not been declared" error */
namespace game::player
{
    typedef unsigned int playerID;

    class Player;
}

#include "../../net/common/include/message.hpp"
#include "./games.hpp"

namespace asio = boost::asio;

namespace game::player {

    /**
     * @brief Player object
     */
    class Player
            : public std::enable_shared_from_this<Player>
   {
   private:
       playerID pID;

        /**
         * @brief TODO
         */
        server::Games& gamesServer;

       /**
        * @brief Name of the player
        */
        std::string nickname;

        /**
         * @brief Socket object
         */
        asio::ip::tcp::socket playerSocket;

        /**
         * @brief The balance of the player
         */
        int balance;

        /**
         * @brief Value indicating if the player is in a running game
         */
        bool isInGame;

        /**
         * @brief Message object
         */
        net::common::Message msg;

        /**
         * @brief Que for outgoing messages
         */
        std::deque<net::common::Message> writeMsgQue;
    public:
        /**
         * @brief Constructor
         * @param games
         * @param n Name of the player
         * @param bal The balance of the player
         * @param sock The socket to do I/O on
         */
        explicit Player(playerID pid, server::Games& games, std::string  n,
                        int bal, asio::ip::tcp::socket sock);

        /**
         * @brief Deconstructor
         */
        ~Player();

        /**
         * @brief Start listening for messages
         */
        void start();
    private:
        /**
         * @brief Read header of incoming message
         */
        void readHeader();

        /**
         * @brief Read the received body
         */
        void readBody();

        /**
         * @brief Send a pong response
         */
        void sendPong();

        /**
         * @brief
         */
         void listGames();

        /**
         * @brief Send a message
         * @param sMsg Message object
         */
        void sendMessage(net::common::Message& sMsg);

        /**
         * @brief Write data to the socket
         */
        void write();
    };
}

#endif //_PLAYER_HPP
