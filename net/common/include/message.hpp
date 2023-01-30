#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP 1

#include <cstdlib>
#include <cstring>

/* Forward declarations to eliminate "'x' was has not been declared" error */
namespace net::common
{
    enum class messageType:char;

    class Message;
}

#define HEADER_LSIZE                                  4
#define HEADER_MSIZE                                  4
#define HEADER_LENGTH     (HEADER_MSIZE + HEADER_LSIZE)
#define BODY_MAX_LENGTH                            1024

namespace net::common
{
    /**
     * @brief Message type enum
     */
    enum class messageType:char {
        InvalidMessage       =  -2,
        InvalidMessageType   =  -1,
        None                 =   0,
        Ping                 =   1,
        Pong                 =   2,
        JoinGame             =   3,
        ListGames            =   4,
        UserRegister         =   5,
        UserRegisterFailed   =   6,
        UserRegisterSuccess  =   7,
        UserLogin            =   8,
        UserLoginFailed      =   9,
        UserLoginSuccess     =  10
    };

    /**
     * @brief Message class
     */
    class Message
    {
    public:
        /**
         * @brief The length of message header in bytes
         */
        static const std::size_t headerLength = HEADER_LENGTH;

        /**
         * @brief The maximum length of the message body
         */
        static const std::size_t bodyMaxLength   = BODY_MAX_LENGTH;
    public:
        /**
         * @brief Constructor
         */
        Message()  = default;

        /**
         * @brief Deconstructor
         */
        ~Message() = default;

        /**
         * @brief Decode the message header
         * @return The length of the received message body in bytes
         */
        std::size_t decodeHeader()
        {
            char mt[HEADER_MSIZE + 1] = "";
            std::strncat(mt, this->data, HEADER_MSIZE);
            // TODO: Make it secure:
            this->recvMt = static_cast<messageType>(std::atoi(mt));

            char len[HEADER_LSIZE + 1] = "";
            std::strncat(len, this->data + HEADER_MSIZE, HEADER_LSIZE);
            this->received = std::atoi(len); // TODO: Make it secure

            if (this->received > bodyMaxLength)
                return 0;

            return this->received;
        }

        /**
         * @brief Encode the message header
         */
        void encodeHeader()
        {
            this->bodyLength = strlen(this->getBody());
            char header[headerLength + 1] = "";
            std::sprintf(header, "%4d%4d", static_cast<int>(this->sendMt),
                         static_cast<int>(this->bodyLength));
            std::memcpy(this->getData(), header, headerLength);
        }

        /**
         * @brief Set the length of the message body
         * @param b The length in bytes
         */
         // TODO: Make this not neceseary
        //void setBodyLength(std::size_t b)
        //{
        //    this->bodyLength = b;
        //}

        /**
         * @return The size of the body in bytes
         */
        [[nodiscard]] std::size_t getBodyLength() const
        {
            return this->bodyLength;
        }

        /**
         * @return The length of the whole message in bytes
         */
        [[nodiscard]] std::size_t length() const
        {
            return headerLength + this->bodyLength;
        }

        /**
         * @return The length of the received body
         */
        [[nodiscard]] std::size_t getReceivedBytes() const
        {
            return this->received;
        }

        /**
         * @return A pointer to the message data
         */
        char *getData()
        {
            return this->data;
        }

        /**
         * @return A pointer to the message body
         */
        char
        *getBody()
        {
            return this->data + headerLength;
        }

        /**
         *
         * @param body
         */
        void
        setBody(std::string body)
        {
            if (body.size() > BODY_MAX_LENGTH)
                return;

            std::copy(body.begin(), body.end(), &(this->data[HEADER_LENGTH]));
            this->data[HEADER_LENGTH + body.size()] = '\0';
        }

        /**
         * @brief Set the send message type
         * @param t The message type
         */
        void setSendMt(messageType t)
        {
            this->sendMt = t;
        }

        /**
         * @return The received message type
         */
        [[nodiscard]] messageType getRecvMt() const
        {
            return this->recvMt;
        }

        /**
         * @brief Clear the message data
         */
        void clearData()
        {
            memset(&(this->data), '\0', BODY_MAX_LENGTH + HEADER_LENGTH);
        }

        /**
         * @param id
         * @return
         */
        //bool joinGame(game::gameID id)
        //{
        //}
    private:
        /**
         * @brief Used to store the message data
         */
        char data[BODY_MAX_LENGTH + HEADER_LENGTH]{};

        /**
         * @brief Used to store the received message header size in bytes
         */
        std::size_t received = 0;

        /**
         * @brief Used to store the received message body size in bytes
         */
        std::size_t bodyLength = 0;

        /**
         * @brief The message type to send
         */
        messageType sendMt = messageType::None;

        /**
         * @brief The received message type
         */
        messageType recvMt = messageType::None;
    };
}

#undef HEADER_LENGTH
#undef BODY_MAX_LENGTH

#endif //_MESSAGE_HPP
