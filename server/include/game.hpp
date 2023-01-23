/* TODO: Write documentation */

#ifndef _GAME_HPP
#define _GAME_HPP 1

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <memory>

/* Forward declarations to eliminate "'x' was has not been declared" error */
namespace game
{
    typedef unsigned int gameID;

    class Game;
}

#include "./player.hpp"

namespace asio = boost::asio;

namespace game
{
    class Game
   {
    private:
        gameID ID;
        int maxPlayers;
        std::string ownerNick;
        std::vector<std::shared_ptr<game::player::Player>> players;
    public:
        explicit Game(gameID ID, int mp, std::string on);
        int join(asio::ip::tcp::socket sock);
        ~Game();
        unsigned long int getPlayerCount();
        [[nodiscard]] int getMaxPlayers() const;
        std::string getOwnerNick() const;
        gameID getGameID() const;
    };
}

#endif //_GAME_HPP
