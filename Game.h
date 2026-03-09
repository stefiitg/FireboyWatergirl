#ifndef OOP_GAME_H
#define OOP_GAME_H

#include <memory>
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Map.h"
#include "Character.h"

class Game {
private:
    std::unique_ptr<sf::RenderWindow> window;
    Map map;

    std::vector<std::unique_ptr<Character>> characters;

    std::vector<std::unique_ptr<Character>> characterPrototypes;
    // starea de de exit
    std::vector<bool> charactersAtExit;
    // pozitii de spawn
    std::vector<sf::Vector2f> spawnPositions;
    // mapare controale per personaj
    struct Controls {
        sf::Keyboard::Key left;
        sf::Keyboard::Key right;
        sf::Keyboard::Key jump;
    };
    std::vector<Controls> characterControls;
    
    bool won = false;
    bool gameOver = false;

    int totalCoins = 0;
    int collectedCoins = 0;
    // pentru afisarea mesajului de castig
    sf::Font winFont;
    sf::Text winText;
    bool winFontLoaded = false;
    //  mesajul de pierdere
    sf::Text loseText;

    void processInput(float dt);
    // Returns true if this character reached its exit during collision handling
    bool handleCollisions(Character& ch);
    void update(float dt);
    void render();
    void resetLevel();
    void handlePlatformCollisions(Character& ch);
    void initializeCharacters();

public:
    explicit Game(int mapW = 14, int mapH = 9);
    // copiere folosind clone() pentru personajele polimorfice
    Game(const Game& other);

    Game& operator=(Game other);

    void swap(Game& other) noexcept {
        using std::swap;
        swap(window, other.window);
        swap(map, other.map);
        swap(characters, other.characters);
        swap(characterPrototypes, other.characterPrototypes);
        swap(charactersAtExit, other.charactersAtExit);
        swap(spawnPositions, other.spawnPositions);
        swap(characterControls, other.characterControls);
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
