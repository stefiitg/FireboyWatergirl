#include "HUD.h"

namespace {
    static std::string levelToString(LevelType lvl) {
        switch (lvl) {
            case LevelType::Level1: return "Level 1";
            case LevelType::Level2: return "Level 2";
            case LevelType::Level3: return "Level 3";
            case LevelType::Level4: return "Level 4";
            default: return "Level ?";
        }
    }
}

HUD::HUD() = default;

void HUD::init(sf::Font& font, unsigned int windowWidth) {
    // Background bar across top
    backgroundBar.setSize(sf::Vector2f(static_cast<float>(windowWidth), 40.f));
    backgroundBar.setPosition(0.f, 0.f);
    backgroundBar.setFillColor(sf::Color(0, 0, 0, 150));

    // Texts
    levelText.setFont(font);
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::White);
    levelText.setString("Current: Level 1");
    levelText.setPosition(10.f, 8.f);

    coinText.setFont(font);
    coinText.setCharacterSize(24);
    coinText.setFillColor(sf::Color::White);
    coinText.setString("Coins: 0 / 0");
    // right align relative to window width; keep 12px padding from right
    sf::FloatRect cb = coinText.getLocalBounds();
    coinText.setOrigin(cb.left + cb.width, cb.top);
    coinText.setPosition(static_cast<float>(windowWidth) - 12.f, 8.f);
}

void HUD::update(LevelType currentLevel, int collectedCoins, int totalCoins) {
    levelText.setString(std::string("Current: ") + levelToString(currentLevel));

    coinText.setString("Coins: " + std::to_string(collectedCoins) + " / " + std::to_string(totalCoins));
    // When the string changes, its width changes; maintain right alignment
    sf::FloatRect cb = coinText.getLocalBounds();
    coinText.setOrigin(cb.left + cb.width, cb.top);
}

void HUD::render(sf::RenderTarget& target) const {
    target.draw(backgroundBar);
    target.draw(levelText);
    target.draw(coinText);
}
