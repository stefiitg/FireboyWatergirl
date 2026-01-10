#ifndef OOP_GAME_H
#define OOP_GAME_H

#include <memory>
#include <utility>
#include <SFML/Graphics.hpp>
#include "Map.h"
#include "Character.h"

class Game {
private:
    std::unique_ptr<sf::RenderWindow> window;
    Map map;
    std::unique_ptr<Character> fireboy;
    std::unique_ptr<Character> watergirl;
    // prototipuri pentru resetări rapide prin clonare
    std::unique_ptr<Character> fireboyPrototype;
    std::unique_ptr<Character> watergirlPrototype;
    bool fireboyAtExit = false;
    bool watergirlAtExit = false;
    bool won = false;
    bool gameOver = false; // Joc pierdut cand un personaj atinge elementul opus
    // Progres monede
    int totalCoins = 0;
    int collectedCoins = 0;
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
    void resetLevel();

public:
    explicit Game(int mapW = 14, int mapH = 9);
    // Copiere profunda folosind clone() pentru personajele polimorfice
    Game(const Game& other);
    // Copy-and-swap assignment
    Game& operator=(Game other);
    // swap pentru copy-and-swap
    void swap(Game& other) noexcept {
        using std::swap;
        swap(window, other.window);
        swap(map, other.map);
        swap(fireboy, other.fireboy);
        swap(watergirl, other.watergirl);
        swap(fireboyPrototype, other.fireboyPrototype);
        swap(watergirlPrototype, other.watergirlPrototype);
        swap(fireboyAtExit, other.fireboyAtExit);
        swap(watergirlAtExit, other.watergirlAtExit);
        swap(won, other.won);
        swap(gameOver, other.gameOver);
        swap(totalCoins, other.totalCoins);
        swap(collectedCoins, other.collectedCoins);
        swap(winFont, other.winFont);
        swap(winText, other.winText);
        swap(winFontLoaded, other.winFontLoaded);
        swap(loseText, other.loseText);
    }
    friend std::ostream& operator<<(std::ostream& os, const Game& g);
    void run();
};

#endif // OOP_GAME_H
