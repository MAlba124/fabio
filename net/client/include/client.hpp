#ifndef _CLIENT_HPP
#define _CLIENT_HPP 1

#include <boost/asio.hpp>
#include <deque>
#include <thread>

#include "../../common/include/message.hpp"
#include "../../common/include/tsDeque.hpp"

namespace asio = boost::asio;

/**
 * Class powering the client
 */
namespace net::client
{
    class Client
    {
    private:
        // TODO: Write doc
        asio::io_context& ioContext;
        asio::ip::tcp::resolver resolver;
        asio::ip::tcp::socket socket;
        asio::ip::tcp::endpoint endpoint;
        net::common::Message msg;
        bool connected = false;
        std::thread iocThread;
        std::deque<net::common::Message> writeMsgQue;
        net::common::TsDeque<net::common::Message> readMsgs;
    public:
        /**
         * @brief Constructor
         * @param ioc io_context to
         */
        Client(asio::io_context& ioc);

        /**
         * @brief Connect to a server
         * @param addr IP address of the server
         * @param port Port to connect to
         */
        void connect(std::string addr, int port);

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
         * @brief Send a ping message to server
         */
        void sendPing();

        /**
         * @return The amount of messages left in readMsgs
         */
        unsigned long int messages();

        /**
         * @return The last message client received
         */
        net::common::Message lastMessage();

        /**
         * @return True if the client is connected to a server
         */
        bool isConnected();
    private:
        // TODO: write doc
        void readHeader();
        void readBody();
        void write();
    };
}

#endif //_CLIENT_HPP
