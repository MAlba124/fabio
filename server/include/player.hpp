#ifndef _PLAYER_HPP
#define _PLAYER_HPP 1

#include <string>

namespace game::player {

    class Player {
    private:
        std::string nickname;
        int balance;
    public:
        explicit Player(std::string  n, int bal);
        ~Player();
    };
}

#endif //_PLAYER_HPP
