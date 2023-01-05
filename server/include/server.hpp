#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <boost/asio.hpp>
#include <string>

namespace asio = boost::asio;

namespace server
{
    class Server {
    private:
        asio::io_context ioc;
        asio::ip::tcp::acceptor ac;
        void doAccept();
    public:
        explicit Server(int portn, const std::string& addr);
        void serve();
        ~Server();
    };
}

#endif //_SERVER_HPP
