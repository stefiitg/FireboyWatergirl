#ifndef OOP_HUD_H
#define OOP_HUD_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Map.h" // for LevelType

class HUD {
private:
    sf::Text levelText;
    sf::Text coinText;
    sf::RectangleShape backgroundBar;

public:
    HUD();

    void init(sf::Font& font, unsigned int windowWidth);
    void update(LevelType currentLevel, int collectedCoins, int totalCoins);
    void render(sf::RenderTarget& target) const;
};

#endif // OOP_HUD_H
