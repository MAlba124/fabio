#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
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
    asio::async_read(this->playerSocket, asio::buffer(this->msg.getData(),
                                  net::common::Message::headerLength),
    [this, self](boost::system::error_code ec, std::size_t)
    {
        if (!ec)
        {
            this->msg.decodeHeader();
            std::cout << "Message type: "
                      << (int)this->msg.getRecvMt()
                      << std::endl;

            if (this->msg.getRecvMt() == net::common::messageType::Ping)
            {
                this->sendPong();
            }

            if (this->msg.getReceivedBytes() > 0)
                this->readBody();
            else
                this->readHeader();
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "Player disconnected";

            /* The socket should already be shutdowned by the client */
            this->playerSocket.close();
        }
    });
}

void
player::Player::readBody()
{
    auto self(shared_from_this());
    asio::async_read(this->playerSocket,
                     asio::buffer(this->msg.getBody(),
                                  this->msg.getReceivedBytes()),
    [this, self](boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
             std::cout << "Received: " << length << " bytes: "
                       << msg.getBody() << std::endl;

            switch (msg.getRecvMt()) {
                case net::common::messageType::Ping:
                    this->sendPong();
                    break;
                case net::common::messageType::None:
                default:
                    break;
            }

             readHeader();
        }
        else
        {
             std::cerr << ec.what() << std::endl;
        }
    });
}

void
player::Player::sendMessage(net::common::Message& sMsg)
{
    bool writeInProgress = !writeMsgQue.empty();
    writeMsgQue.push_back(sMsg);
    if (!writeInProgress) write();
}

void
player::Player::write()
{
    asio::async_write(this->playerSocket,
          asio::buffer(writeMsgQue.front().getData(),
                       writeMsgQue.front().length()),
          [this](boost::system::error_code ec, std::size_t /*length*/)
          {
              if (!ec)
              {
                  this->writeMsgQue.pop_front();
                  if (!this->writeMsgQue.empty())
                  {
                      this->write();
                  }
              }
              else
              {
                  BOOST_LOG_TRIVIAL(error) << "Failed to write to client :"
                                           << ec.what();
                  this->playerSocket.close();
              }
          });

}


void
player::Player::sendPong()
{
    this->msg.clearData();
    std::memcpy(this->msg.getBody(), "!\0", 2);
    this->msg.setSendMt(net::common::messageType::Pong);
    this->msg.encodeHeader();
    this->sendMessage(msg);
}


game::player::Player::~Player()
    = default;