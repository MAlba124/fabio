#include <vector>
#include <memory>

#include "./include/games.hpp"

server::Games::Games(int maxGames, int maxPlayers)
    : maxGames(maxGames),
      maxPlayers(maxPlayers)
{
}

server::gamesError
server::Games::playerJoinGame(std::shared_ptr<game::player::Player> p,
                              game::gameID ID)
{
    unsigned long int index = this->gs.size();
    for (int i = 0; i < (int)this->gs.size(); i++)
    {
        if (this->gs[i])
            if (this->gs[i]->getGameID() == ID)
            {
                index = i;
                break;
            }
    }

    if (index == this->gs.size())
        return gamesError::DOES_NOT_EXIST;
    else if (this->gs[index]->getPlayerCount() >=
            (unsigned long int)this->gs[index]->getMaxPlayers())
        return gamesError::FULL;



    return gamesError::SUCCESS;
}

std::vector<server::GameInfo>
server::Games::getAllGamesInfo()
{
    std::vector<server::GameInfo> gi;

    gi.reserve(this->gs.size());
    for (const auto& x : this->gs)
    {
        gi.push_back(
                server::GameInfo {
                        .ID         = x->getGameID(),
                        .ownerNick  = x->getOwnerNick(),
                        .maxPlayers = x->getMaxPlayers(),
                        .players    = static_cast<int> (x->getPlayerCount())
                }
        );
    }

    return gi;
}
