/*
 * Modified from:
 * https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio/example/cpp11/chat/chat_message.hpp
 */

#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP 1

#include <cstdlib>
#include <cstring>

#define HEADER_LSIZE                                  4
#define HEADER_MSIZE                                  4
#define HEADER_LENGTH     (HEADER_MSIZE + HEADER_LSIZE)
#define BODY_MAX_LENGTH                             512

namespace net::common
{
    enum class messageType:char {
        InvalidMessage     = -2,
        InvalidMessageType = -1,
        None               = 0,
        Ping               = 1,
        Pong               = 2
    };

    class Message
    {
    public:
        static const std::size_t headerLength = HEADER_LENGTH;
        static const std::size_t bodyMaxLength   = BODY_MAX_LENGTH;
    public:
        Message()  = default;
        ~Message() = default;

        std::size_t decodeHeader()
        {
            char mt[HEADER_MSIZE + 1] = "";
            std::strncat(mt, this->data, HEADER_MSIZE);
            this->recvMt = static_cast<messageType>(std::atoi(mt)); // TODO: Make it secure

            char len[HEADER_LSIZE + 1] = "";
            std::strncat(len, this->data + HEADER_MSIZE, HEADER_LSIZE);
            this->received =  std::atoi(len); // TODO: Make it secure

            if (this->received > bodyMaxLength)
                return 0;

            return this->received;
        }

        void encodeHeader()
        {
            char header[headerLength + 1] = "";
            std::sprintf(header, "%4d%4d", static_cast<int>(this->sendMt),
                         static_cast<int>(this->bodyLength));
            std::memcpy(this->getData(), header, headerLength);
        }

        void setBodyLength(std::size_t b)
        {
            this->bodyLength = b;
        }

        [[nodiscard]] std::size_t getBodyLength() const
        {
            return this->bodyLength;
        }

        [[nodiscard]] std::size_t length() const
        {
            return headerLength + this->bodyLength;
        }

        [[nodiscard]] std::size_t getReceivedBytes() const
        {
            return this->received;
        }

        char *getData()
        {
            return this->data;
        }

        char *getBody()
        {
            return this->data + headerLength;
        }

        void setSendMt(messageType t)
        {
            this->sendMt = t;
        }

        [[nodiscard]] messageType getRecvMt() const
        {
            return this->recvMt;
        }

        void clearData()
        {
            memset(&(this->data), '\0', BODY_MAX_LENGTH + HEADER_LENGTH);
        }
    private:
        char data[BODY_MAX_LENGTH + HEADER_LENGTH]{};
        std::size_t received = 0;
        std::size_t bodyLength = 0;
        messageType sendMt = messageType::None;
        messageType recvMt = messageType::None;
    };
}

#undef HEADER_LENGTH
#undef BODY_MAX_LENGTH

#endif //_MESSAGE_HPP
