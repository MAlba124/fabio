#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <utility>
#include <string>
#include <vector>
#include <iostream>

#include "./include/player.hpp"
#include "./include/server.hpp"
#include "./include/games.hpp"

namespace player = game::player;
namespace asio = boost::asio;

player::Player::Player(player::playerID pid, server::Games& games,
                       std::string n, int bal, asio::ip::tcp::socket sock)
    : pID(pid),
      gamesServer(games),
      nickname(std::move(n)),
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
            BOOST_LOG_TRIVIAL(trace) << "Received message (" << this->pID
                                     << ") : "
                                     << "Type: "
                                     << (int)this->msg.getRecvMt();

            if (this->msg.getReceivedBytes() > 0)
                this->readBody();
            else
                this->readHeader();
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "Client disconnected ("
                                    << this->pID << ')';

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
    [this, self](boost::system::error_code ec, std::size_t)
    {
        if (!ec)
        {
            switch (msg.getRecvMt()) {
                case net::common::messageType::Ping:
                    this->sendPong();
                    break;
                case net::common::messageType::ListGames:
                    this->listGames();
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
    if (!writeInProgress)
    {
        write();
    }
}

void
player::Player::write()
{
    asio::async_write(this->playerSocket,
          asio::buffer(writeMsgQue.front().getData(),
                       writeMsgQue.front().length()),
          [this](boost::system::error_code ec, std::size_t length)
          {
              if (!ec)
              {
                  BOOST_LOG_TRIVIAL(trace) << "Wrote: "
                                           << length
                                           << " bytes to client ("
                                           << this->pID
                                           << ')';
                  this->writeMsgQue.pop_front();
                  if (!this->writeMsgQue.empty())
                  {
                      this->write();
                  }
              }
              else
              {
                  BOOST_LOG_TRIVIAL(error) << "Failed to write to client ("
                                           << this->pID
                                           << ") :"
                                           << ec.what();
                  this->playerSocket.close();
              }
          });

}

void
player::Player::sendPong()
{
    this->msg.clearData();
    std::memcpy(this->msg.getBody(), "TEST123", 7);
    this->msg.setBodyLength(8);
    this->msg.setSendMt(net::common::messageType::Pong);
    this->msg.encodeHeader();
    BOOST_LOG_TRIVIAL(trace) << this->msg.getBodyLength();
    BOOST_LOG_TRIVIAL(trace) << this->msg.getData();
    this->sendMessage(msg);
}

void
player::Player::listGames()
{
    std::vector<server::GameInfo> gi = this->gamesServer.getAllGamesInfo();
    BOOST_LOG_TRIVIAL(trace) << "Number of games: " << gi.size();
}

game::player::Player::~Player()
    = default;