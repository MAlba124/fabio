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
                switch (this->msg.getRecvMt()) {
                    case net::common::messageType::None:
                        break;
                    case net::common::messageType::Ping:
                        this->sendPong();
                        break;
                    case net::common::messageType::ListGames:
                        this->listGames();
                        break;
                    //case net::common::messageType::InvalidMessage:
                    //    break;
                    //case net::common::messageType::InvalidMessageType:
                        break;
                    case net::common::messageType::Pong:
                        break;
                    //case net::common::messageType::JoinGame:
                    //    break;
                    //case net::common::messageType::UserLogin:
                    //    break;
                    default:
                        this->sendInvalidMessageType();
                        break;
                }

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
                case net::common::messageType::UserRegister:
                    this->registerUser();
                    break;
                case net::common::messageType::UserLogin:
                    this->login();
                    break;
                case net::common::messageType::None:
                default:
                    break;
            }

             readHeader();
        }
        else
        {
            BOOST_LOG_TRIVIAL(warning) << ec.what() << " (" << this->pID << ')';
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
player::Player::sendBasic(net::common::messageType type)
{
    this->msg.clearData();
    this->msg.setSendMt(type);
    this->msg.encodeHeader();
    this->sendMessage(msg);
}

void
player::Player::sendPong()
{
    this->sendBasic(net::common::messageType::Pong);
    BOOST_LOG_TRIVIAL(debug) << "Sent Pong (" << this->pID << ')';
}

void player::Player::sendInvalidMessage()
{
    this->sendBasic(net::common::messageType::InvalidMessage);
    BOOST_LOG_TRIVIAL(debug) << "Sent InvalidMessage (" << this->pID << ')';
}

void player::Player::sendInvalidMessageType()
{
    this->sendBasic(net::common::messageType::InvalidMessageType);
    BOOST_LOG_TRIVIAL(debug) << "Sent InvalidMessageType (" << this->pID << ')';
}

void
player::Player::sendRegistrationFailed()
{
    this->sendBasic(net::common::messageType::UserRegisterFailed);
    BOOST_LOG_TRIVIAL(debug) << "Sent UserRegisterFailed (" << this->pID << ')';
}

void
player::Player::sendRegistrationSuccess()
{
    this->sendBasic(net::common::messageType::UserRegisterSuccess);
    BOOST_LOG_TRIVIAL(debug) << "Sent UserRegisterSuccess ("
                             << this->pID << ')';
}

void
player::Player::sendLoginFailed()
{
    this->sendBasic(net::common::messageType::UserLoginFailed);
    BOOST_LOG_TRIVIAL(debug) << "Sent UserLoginFailed (" << this->pID << ')';
}

void
player::Player::sendLoginSuccess()
{
    this->sendBasic(net::common::messageType::UserLoginSuccess);
    BOOST_LOG_TRIVIAL(debug) << "Sent UserLoginSuccess (" << this->pID << ')';
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
    server::db::User u;

    ss >> nick >> pass;
    if (ss.fail())
    {
        BOOST_LOG_TRIVIAL(warning) << "Received invalid registerUser message ("
                                   << this->pID << ')';
        this->sendInvalidMessage();
        return;
    }

    if (this->db->userExist(nick))
    {
        BOOST_LOG_TRIVIAL(debug) << "User already exists!";
        this->sendRegistrationFailed();
        return;
    }

    // TODO: add default balance
    if (!(u = this->db->userAdd(nick, pass, 1000)).err) {
        BOOST_LOG_TRIVIAL(debug) << "Registered user: Nick: " << nick
                                << " Pass: " << pass
                                << " (" << this->pID << ')';
        this->sendRegistrationSuccess();
        return;
    }

    BOOST_LOG_TRIVIAL(warning) << "Failed to register user ("
                               << this->pID << ')';
    this->sendRegistrationFailed();
}

game::player::Player::~Player()
    = default;

void
player::Player::login()
{
    std::basic_stringstream<char> ss(this->msg.getBody());
    std::string nick, pass;
    server::db::User u;

    ss >> nick >> pass;
    if (ss.fail())
    {
        BOOST_LOG_TRIVIAL(warning) << "Received invalid login message ("
                                   << this->pID << ')';
        this->sendInvalidMessage();
        return;
    }

    if (!(u = this->db->userValidate(nick, pass)).err)
    {
        this->nickname = u.nick;
        this->balance = u.balance;
        BOOST_LOG_TRIVIAL(info) << "User login successfull: Nick: " << nick
                                << " Balance: " << u.balance
                                << " (" << this->pID << ')';
        this->sendLoginSuccess();
        this->isLoggedIn = true;
        return;
    }

    BOOST_LOG_TRIVIAL(warning) << "User login failed: Nick: " <<  nick
                            << " (" << this->pID << ')';
    this->sendLoginFailed();
}