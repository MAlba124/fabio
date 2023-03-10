#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>

#include "./include/client.hpp"

namespace client = net::client;
namespace asio = boost::asio;

client::Client::Client(asio::io_context& ioc)
    : ioContext(ioc),
      resolver(ioc),
      socket(ioc)
{
}

void
client::Client::connect(std::string addr, int port = 3355)
{
    this->endpoint =
            asio::ip::tcp::endpoint(asio::ip::address::from_string(addr), port);

    boost::system::error_code ec;
    this->socket.connect(this->endpoint, ec);

    if ((this->connected = !ec.failed()))
        this->readHeader();
}


void
client::Client::disconnect()
{
    if (this->socket.is_open())
        this->close();
    this->connected = false;
}

void
client::Client::close()
{
    if (this->isConnected())
        asio::post(this->ioContext,
           [this]()
           {
                boost::system::error_code ec; // TODO: Do something with the error
                socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
                socket.close();
                ioContext.stop();
           });
}

void
client::Client::sendMessage(net::common::Message &sMsg)
{
    asio::post(this->ioContext,
       [this, sMsg]()
       {
            bool writeInProgress = !writeMsgQue.empty();
            writeMsgQue.push_back(sMsg);
            if (!writeInProgress) write();
       });
}

void
client::Client::readHeader()
{
    asio::post(this->ioContext,
   [this]()
   {
       asio::async_read(this->socket,
        asio::buffer(this->msg.getData(), net::common::Message::headerLength),
        [this](boost::system::error_code ec, std::size_t)
        {
            if (!ec)
            {
                this->msg.decodeHeader();
                /* If the received message type is ping, respond with a
                 * pong message */
                if (this->msg.getRecvMt() == net::common::messageType::Ping)
                {
                    // TODO: add to separate function:
                    net::common::Message m;
                    std::memcpy(m.getBody(), " ", 1);
                    m.setSendMt(net::common::messageType::Pong);
                    m.encodeHeader();
                    this->sendMessage(m);
                }

                if (this->msg.getReceivedBytes() > 0)
                    this->readBody();
                else
                    this->readHeader();
            }
            else
            {
                this->close();
                std::cerr << ec.what() << std::endl;
            }
        });
    });
}

void
client::Client::readBody()
{
    asio::post(this->ioContext,
    [this]()
    {
        asio::async_read(this->socket,
             asio::buffer(this->msg.getBody(), this->msg.getReceivedBytes()),
             [this](boost::system::error_code ec, std::size_t /* len */)
             {
                 if (!ec)
                 {
                     this->readMsgs.push_back(this->msg);
                     std::basic_stringstream<char> ss(this->msg.getBody());
                     std::string ts;
                     ss >> ts;
                     if (ss.fail())
                         std::cerr << "Error: Invalid input" << std::endl;
                     //else if (ss.eof()
                     // while (!s.eof()) { ... }
                     //    std::cout << "Error: Unexpected end of input"
                     //              << std::endl;

                     this->readHeader();
                 }
                 else
                 {
                     std::cerr << ec.what() << std::endl;
                 }
             });
    });
}

void
client::Client::write()
{
    asio::post(this->ioContext,
    [this]()
    {
       asio::async_write(this->socket,
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
                 BOOST_LOG_TRIVIAL(error) << ec.what();
                 this->socket.close();
             }
         });
    });
}

void
client::Client::sendPing()
{
    this->msg.clearData();
    this->msg.setSendMt(net::common::messageType::Ping);
    this->msg.encodeHeader();

    this->sendMessage(msg);
}

unsigned long int
client::Client::messages()
{
    return this->readMsgs.size();
}

net::common::Message
client::Client::lastMessage()
{
    net::common::Message m = this->readMsgs.front();
    this->readMsgs.pop_front();
    return m;
}

bool
client::Client::isConnected()
{
    //return this->socket.is_open() && this->connected;
    return this->socket.is_open();
}

void
client::Client::uregister(std::string nick, std::string pass)
{
    this->msg.clearData();
    this->msg.setSendMt(net::common::messageType::UserRegister);
    this->msg.setBody(nick + ' ' + pass);
    this->msg.encodeHeader();

    this->sendMessage(this->msg);
}

void
client::Client::login(std::string nick, std::string pass)
{
    this->msg.clearData();
    this->msg.setSendMt(net::common::messageType::UserLogin);
    this->msg.setBody(nick + ' ' + pass);
    this->msg.encodeHeader();

    this->sendMessage(this->msg);
}

void
client::Client::sendPong()
{
    this->msg.clearData();
    this->msg.setSendMt(net::common::messageType::Pong);
    this->msg.encodeHeader();

    this->sendMessage(msg);
}

client::Client::~Client()
{
    if (this->socket.is_open())
        this->close();
}
