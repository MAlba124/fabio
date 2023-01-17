#ifndef _PLAYER_HPP
#define _PLAYER_HPP 1

#include <boost/asio.hpp>
#include <string>
#include <memory>
#include <deque>

#include "../../net/common/include/message.hpp"

namespace asio = boost::asio;

namespace game::player {

    /**
     * @brief Player object
     */
    class Player
            : public std::enable_shared_from_this<Player>
   {
    private:
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
         * @param n Name of the player
         * @param bal The balance of the player
         * @param sock The socket to do I/O on
         */
        explicit Player(std::string  n, int bal, asio::ip::tcp::socket sock);

        /**
         * @brief Decontrutor
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
