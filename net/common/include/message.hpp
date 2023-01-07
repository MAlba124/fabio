/*
 * Modified from:
 * https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio/example/cpp11/chat/chat_message.hpp
 */

#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP

#include <cstdlib>
#include <cstring>

#define HEADER_LENGTH   4
#define BODY_LENGTH   512

namespace net::common
{
    class Message
    {
    public:
        const std::size_t headerLength = HEADER_LENGTH;
        const std::size_t bodyLength   = BODY_LENGTH;
    public:
        Message()  = default;
        ~Message() = default;

        std::size_t decodeHeader()
        {
            char header[this->headerLength + 1] = "";

            std::strncat(header, this->data, headerLength);
            this->received =  std::atoi(header);
            if (this->received > bodyLength)
                return 0;

            return this->received;
        }

        char *getData()
        {
            return this->data;
        }

        std::size_t getReceivedBytes()
        {
            return this->received;
        }
    private:
        char data[BODY_LENGTH + HEADER_LENGTH];
        std::size_t received = 0;
    };
}

#undef HEADER_LENGTH
#undef BODY_LENGTH

#endif //_MESSAGE_HPP
