#ifndef _CLIENT_HPP
#define _CLIENT_HPP 1

#include <boost/asio.hpp>
#include <deque>
#include <thread>

#include "../../common/include/message.hpp"
//#include "../../common/include/tsDeque.hpp"

namespace asio = boost::asio;

/**
 * Class powering the client
 */
namespace net::client
{
    class Client
    {
    private:
        asio::io_context& ioContext;
        asio::ip::tcp::resolver resolver;
        asio::ip::tcp::socket socket;
        asio::ip::tcp::endpoint endpoint;
        net::common::Message msg;
        bool connected = false;
        std::thread iocThread;
        std::deque<net::common::Message> writeMsgQue;
    public:
        /**
         * @param ioc io_context to
         * @param addr IP address of the server
         * @param port Port to connect to
         */
        Client(asio::io_context& ioc, std::string addr, int port);

        /**
         * Destructor
         */
        ~Client();

        /**
         * Close the connection to the server
         */
        void close();

        /**
         * Send a message to the server
         * @param sMsg The message object to send
         */
        void writeMsg(net::common::Message& sMsg);

        /**
         * @return True if the client is connected to a server
         */
        bool isConnected();
    private:
        void connect();
        void readHeader();
        void readBody();
        void write();
    };
}

#endif //_CLIENT_HPP
