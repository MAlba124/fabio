#ifndef _GAME_HPP
#define _GAME_HPP 1

#include <boost/asio.hpp>
#include <string>
#include <vector>

#include "player.hpp"

namespace asio = boost::asio;

namespace game {
    class Game {
    private:
        int maxPlayers;
        std::string ownerNick;
        std::vector<game::player::Player> players;
    public:
        explicit Game(int mp, std::string on);
        int join(asio::ip::tcp::socket sock);
        ~Game();
    };
}

#endif //_GAME_HPP
