#include <boost/asio.hpp>
#include <utility>
#include <string>
#include <iostream>

#include "./include/player.hpp"

namespace player = game::player;
namespace asio = boost::asio;

player::Player::Player(std::string n, int bal, asio::ip::tcp::socket sock)
    : nickname(std::move(n)),
      balance(bal),
      playerSocket(std::move(sock))
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
                     asio::buffer(this->msg.getData(), this->msg.headerLength),
    [this, self](boost::system::error_code ec, std::size_t)
    {
        this->msg.decodeHeader();
        if (!ec && this->msg.getReceivedBytes())
        {
            readBody();
        }
        else
        {
            std::cerr << ec.what() << std::endl;
        }
    });
}

void
player::Player::readBody()
{
    auto self(shared_from_this());
    asio::async_read(this->playerSocket,
                     asio::buffer(this->msg.getData(), this->msg.bodyLength),
    [this, self](boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
             std::cout << "Received: " << length << " bytes: "
                       << msg.getData() << std::endl;

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
