#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>
#include <thread>

#include "./include/client.hpp"

namespace client = net::client;
namespace asio = boost::asio;

client::Client::Client(asio::io_context& ioc, std::string addr, int port = 3355)
    : ioContext(ioc),
      resolver(ioc),
      socket(ioc),
      endpoint(asio::ip::address::from_string(addr), port)
{
    this->connect();
}

void
client::Client::connect()
{

    boost::system::error_code ec;
    this->socket.connect(this->endpoint, ec);

    if ((this->connected = !ec.failed()))
        this->readHeader();
}

void
client::Client::close()
{
    asio::post(this->ioContext,
       [this]()
       {
            boost::system::error_code ec; // TODO: Do something with the error
            socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            socket.close();
       });
}

void
client::Client::writeMsg(net::common::Message& sMsg)
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
                    this->writeMsg(m);
                }

                this->readBody();
            }
            else
            {
                this->close();
                BOOST_LOG_TRIVIAL(error) << ec.what();
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
             [this](boost::system::error_code ec, std::size_t)
             {
                 if (!ec)
                 {
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

bool
client::Client::isConnected()
{
    //return this->connected;
    return this->socket.is_open();
}

client::Client::~Client()
{
    if (this->socket.is_open())
        this->close();
}