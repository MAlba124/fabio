#include <utility>
#include <string>

#include "./include/player.hpp"

namespace player = game::player;

player::Player::Player(std::string n, int bal)
    : nickname(std::move(n)),
      balance(bal)
{
}

game::player::Player::~Player()
    = default;
