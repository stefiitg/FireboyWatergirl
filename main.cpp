
#include <iostream>
#include "Game.h"
#include "GameExceptions.h"

int main() {
    try {
        Game game(14, 9);
        std::cout << game << std::endl;
        game.run();
        return 0;
    } catch (const GameError& ge) {
        std::cerr << "Game error: " << ge.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 2;
    }
}