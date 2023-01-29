#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <thread>

#include "../../net/common/include/message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<net::common::Message> chat_message_queue;

class Client
{
public:
    Client(boost::asio::io_context& io_context,
                const tcp::resolver::results_type& endpoints)
            : io_context_(io_context),
              socket_(io_context)
    {
        do_connect(endpoints);
    }

    void write(const net::common::Message& msg)
    {
        boost::asio::post(io_context_,
          [this, msg]()
          {
              bool write_in_progress = !write_msgs_.empty();
              write_msgs_.push_back(msg);
              if (!write_in_progress)
              {
                  do_write();
              }
          });
    }

    void close()
    {
        boost::asio::post(io_context_, [this]() { socket_.close(); });
    }

private:
    void do_connect(const tcp::resolver::results_type& endpoints)
    {
        boost::asio::async_connect(socket_, endpoints,
                   [this](boost::system::error_code ec, tcp::endpoint)
                   {
                       if (!ec)
                       {
                           do_read_header();
                       }
                   });
    }

    void do_read_header()
    {
        boost::asio::async_read(socket_,
                boost::asio::buffer(read_msg_.getData(), read_msg_.headerLength),
                [this](boost::system::error_code ec, std::size_t /*length*/)
                {
                    if (!ec && read_msg_.decodeHeader())
                    {
                        do_read_body();
                    }
                    else
                    {
                        socket_.close();
                    }
                });
    }

    void do_read_body()
    {
        boost::asio::async_read(socket_,
                boost::asio::buffer(read_msg_.getBody(), read_msg_.getReceivedBytes()),
                [this](boost::system::error_code ec, std::size_t /*length*/)
                {
                    if (!ec)
                    {
                        std::cout.write(read_msg_.getBody(), read_msg_.getReceivedBytes());
                        std::cout << "\n";
                        do_read_header();
                    }
                    else
                    {
                        socket_.close();
                    }
                });
    }

    void do_write()
    {
        boost::asio::async_write(socket_,
                                 boost::asio::buffer(write_msgs_.front().getData(),
                                                     write_msgs_.front().length()),
                 [this](boost::system::error_code ec, std::size_t /*length*/)
                 {
                     if (!ec)
                     {
                         write_msgs_.pop_front();
                         if (!write_msgs_.empty())
                         {
                             do_write();
                         }
                     }
                     else
                     {
                         socket_.close();
                     }
                 });
    }

private:
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    net::common::Message read_msg_;
    chat_message_queue write_msgs_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(argv[1], argv[2]);
        Client c(io_context, endpoints);

        std::thread t([&io_context](){ io_context.run(); });

        net::common::Message msg;

        std::string line = "Ping!";
        //msg.setBodyLength(line.size());
        //std::memcpy(msg.getBody(), line.data(), msg.getBodyLength());
        //msg.getBody() = line.data();
        msg.setSendMt(net::common::messageType::Ping);
        msg.encodeHeader();
        c.write(msg);

        c.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
