#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <utility>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "./include/player.hpp"
#include "./include/games.hpp"

namespace player = game::player;
namespace asio = boost::asio;

player::Player::Player(player::playerID pid,
                       std::shared_ptr<server::Games> games,
                       std::string n,
                       int bal,
                       asio::ip::tcp::socket sock,
                       std::shared_ptr<server::db::DB> db)
    : pID(pid),
      gamesServer(std::move(games)),
      nickname(std::move(n)),
      playerSocket(std::move(sock)),
      balance(bal),
      isInGame(false),
      db(db)
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
            {
                this->readBody();
            }
            else
            {
                if (this->msg.getRecvMt() == net::common::messageType::Ping)
                    this->sendPong();
                this->readHeader();
            }
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "Client disconnected ("
                                    << this->pID << ')';

            /* It is expected that the socket has already been shut down by
             * the client */
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
                case net::common::messageType::UserRegister:
                    this->registerUser();
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
    this->msg.setSendMt(net::common::messageType::Pong);
    this->msg.encodeHeader();
    this->sendMessage(msg);
    BOOST_LOG_TRIVIAL(debug) << "Sent pong (" << this->pID << ')';
}

void
player::Player::listGames()
{
    std::vector<server::GameInfo> gi = this->gamesServer->getAllGamesInfo();
    BOOST_LOG_TRIVIAL(trace) << "Number of games: " << gi.size();
}

void
player::Player::registerUser()
{
    std::basic_stringstream<char> ss(this->msg.getBody());
    std::string nick, pass;
    ss >> nick >> pass;
    if (ss.fail())
    {
        BOOST_LOG_TRIVIAL(warning) << "Received invalid registerUser request ("
                                   << this->pID << ')';
        return;
    }

    if (this->db->userExist(nick))
        BOOST_LOG_TRIVIAL(debug) << "User already exists!";
    else
        BOOST_LOG_TRIVIAL(debug) << "Nickname: " << nick << " Password: " << pass;
}

game::player::Player::~Player()
    = default;