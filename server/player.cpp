#include <boost/asio.hpp>
#include <utility>
#include <string>
#include <iostream>

#include "./include/player.hpp"

namespace player = game::player;
namespace asio = boost::asio;

player::Player::Player(std::string n, int bal, asio::ip::tcp::socket sock)
    : nickname(std::move(n)),
      playerSocket(std::move(sock)),
      balance(bal),
      isInGame(false)
{
}

void
player::Player::start()
{
    this->readHeader();
}

void
player::Player::readHeader()
{
    auto self(shared_from_this());
    asio::async_read(this->playerSocket,
                     asio::buffer(this->msg.getData(), net::common::Message::headerLength),
    [this, self](boost::system::error_code ec, std::size_t)
    {
        this->msg.decodeHeader();
        if (!ec && this->msg.getReceivedBytes())
        {
            std::cout << "Message type: " << (int)this->msg.getRecvMt() << std::endl;
            readBody();
        }
        else
        {
            if (isInGame) std::cout << "Left game\n";
            playerSocket.close();
            std::cout << "Client left\n";
        }
    });
}

void
player::Player::readBody()
{
    auto self(shared_from_this());
    asio::async_read(this->playerSocket,
                     asio::buffer(this->msg.getBody(), this->msg.getReceivedBytes()),
    [this, self](boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
             std::cout << "Received: " << length << " bytes: "
                       << msg.getBody() << std::endl;

             readHeader();
        }
        else
        {
             std::cerr << ec.what() << std::endl;
        }
    });
}


game::player::Player::~Player()
    = default;