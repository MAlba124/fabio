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
        int balance;
        asio::ip::tcp::socket playerSocket;
        net::common::Message msg;
        //char data[1024];
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
