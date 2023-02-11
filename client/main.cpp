#include <boost/asio.hpp>

#include "./include/game.hpp"

int main(void)
{
    fabio::Game game;
    game.run();
    game.cleanup();

    return 0;
}
