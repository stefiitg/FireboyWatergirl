// main.cpp minimal după separarea în .h/.cpp
#include <iostream>
#include "Game.h"

int main() {
    Game game(14, 9);
    std::cout << game << std::endl;
    game.run();
    return 0;
}