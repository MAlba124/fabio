#ifndef _PLAYER_HPP
#define _PLAYER_HPP 1

#include <boost/asio.hpp>
#include <string>
#include <memory>

#include "../../net/common/include/message.hpp"

namespace asio = boost::asio;

namespace game::player {

    class Player
            : public std::enable_shared_from_this<Player>
   {
    private:
        std::string nickname;
        asio::ip::tcp::socket playerSocket;
        int balance;
        bool isInGame;
        net::common::Message msg;
    public:
        explicit Player(std::string  n, int bal, asio::ip::tcp::socket sock);
        ~Player();
        void start();
    private:
        void readHeader();
        void readBody();
    };
}

#endif //_PLAYER_HPP
