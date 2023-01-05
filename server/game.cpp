#include <utility>
#include <vector>

#include "./include/game.hpp"
#include "./include/player.hpp"

game::Game::Game(int mp, std::string on)
    : maxPlayers(mp),
      ownerNick(std::move(on))
{
}

int
game::Game::join(asio::ip::tcp::socket sock)
{
    if ((int)this->players.size() >= this->maxPlayers)
        return 1; // Enum with for error codes?

    this->players.emplace_back("Default", 1000);
    return 0;
}

game::Game::~Game()
    = default;
