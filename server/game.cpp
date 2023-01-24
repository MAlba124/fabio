#include <utility>

#include "./include/game.hpp"

game::Game::Game(game::gameID ID, int mp, std::string on)
    : ID(ID),
      maxPlayers(mp),
      ownerNick(std::move(on))
{
}

//int
//game::Game::join(std::shared_ptr<game::player::Player> p)

int
game::Game::join(asio::ip::tcp::socket)
{
    /* BLOAT: should not be necessary */
    if ((int)this->players.size() >= this->maxPlayers)
        return 1; // Enum with for error codes?

    //this->players.emplace_back("Default", 1000);
    return 0;
}

unsigned long int
game::Game::getPlayerCount()
{
    return this->players.size();
}

int
game::Game::getMaxPlayers() const
{
    return this->maxPlayers;
}

std::string
game::Game::getOwnerNick() const
{
    return this->ownerNick;
}

game::gameID
game::Game::getGameID() const
{
    return this->ID;
}


game::Game::~Game()
    = default;
