#ifndef OOP_GAME_H
#define OOP_GAME_H

#include <memory>
#include <SFML/Graphics.hpp>
#include "Map.h"
#include "Character.h"

class Game {
private:
    std::unique_ptr<sf::RenderWindow> window;
    Map map;
    std::unique_ptr<Character> fireboy;
    std::unique_ptr<Character> watergirl;
    bool fireboyAtExit = false;
    bool watergirlAtExit = false;
    bool won = false;
    bool gameOver = false; // Joc pierdut cand un personaj atinge elementul opus
    // Elemente UI pentru afisarea mesajului de castig
    sf::Font winFont;
    sf::Text winText;
    bool winFontLoaded = false;
    // Elemente UI pentru mesajul de pierdere
    sf::Text loseText;

    void processInput(float dt);
    void handleCollisions(Character& ch, const sf::Vector2f& respawnPos,
                          bool& reachedExitForCharacter, TileType whatExit);
    void update(float dt);
    void render();

public:
    explicit Game(int mapW = 14, int mapH = 9);
    friend std::ostream& operator<<(std::ostream& os, const Game& g);
    void run();
};

#endif // OOP_GAME_H
