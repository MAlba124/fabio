/* TODO: Write doc */

#ifndef _GAMES_HPP
#define _GAMES_HPP 1

#include <vector>
#include <memory>
#include <string>

namespace server
{
    enum gamesError
    {
        SUCCESS,
        DOES_NOT_EXIST,
        FULL
    };

    struct GameInfo;

    class Games;
}

#include "./game.hpp"

namespace server
{
    struct GameInfo
    {
        game::gameID ID;
        std::string ownerNick;
        int maxPlayers;
        int players;
    };

    class Games
            : public std::enable_shared_from_this<Games>
    {
    private:
        int maxGames;
        int maxPlayers;
        std::vector<std::shared_ptr<game::Game>> gs;
    public:
        explicit Games(int maxGames, int maxPlayers);
        ~Games() = default;

        gamesError playerJoinGame(std::shared_ptr<game::player::Player> p,
                                  game::gameID ID);

        std::vector<GameInfo> getAllGamesInfo();
        //gamesError createGame()
    };
}

#endif //_GAMES_HPP
