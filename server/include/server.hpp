#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace server
{
    class Server {
    private:
        asio::io_context ioc;
        asio::ip::tcp::acceptor ac;
        void doAccept(void);
    public:
        Server(int portn);
        void serve(void);
        ~Server(void);
    };
}

#endif //_SERVER_HPP
