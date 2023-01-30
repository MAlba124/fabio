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

namespace server
{
    class Games;
    struct GameInfo;
    class Server;
}

#include "../../net/common/include/message.hpp"
#include "./database.hpp"
//#include "./server.hpp"
//#include "./games.hpp"

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
        * @brief Player ID
        */
       playerID pID;

       /**
         * @brief TODO
         */
       //server::Games& gamesServer;
       std::shared_ptr<server::Games> gamesServer;

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

        std::shared_ptr<server::db::DB> db;

        /**
         * @brief Message object
         */
        net::common::Message msg;

        /**
         * @brief Que for outgoing messages
         */
        std::deque<net::common::Message> writeMsgQue;

        bool isLoggedIn = false;
    public:
        /**
         * @brief Constructor
         * @param games
         * @param n Name of the player
         * @param bal The balance of the player
         * @param sock The socket to do I/O on
         */
        explicit Player(player::playerID pid,
                        std::shared_ptr<server::Games> games,
                        std::string n,
                        int bal,
                        asio::ip::tcp::socket sock,
                        std::shared_ptr<server::db::DB> db);

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

        void sendBasic(net::common::messageType type);

        /**
         * @brief Send a pong response
         */
        void sendPong();

        void sendInvalidMessage();

        void sendInvalidMessageType();

        void sendRegistrationFailed();

        void sendRegistrationSuccess();

        void sendLoginFailed();

        void sendLoginSuccess();


        /**
         * @brief
         */
         void listGames();

         void registerUser();

         void login();

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
