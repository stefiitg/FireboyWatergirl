#ifndef OOP_CHARACTERFACTORY_H
#define OOP_CHARACTERFACTORY_H

#include "Character.h"
#include <memory>
#include <stdexcept>
#include <SFML/Graphics.hpp>

// Factory Method Pattern: create characters based on enum type
enum class PlayerType { Fireboy, Watergirl, Earthboy, Airgirl };

class CharacterFactory {
public:
    static std::unique_ptr<Character> createCharacter(PlayerType type, const sf::Vector2f& spawnPos) {
        switch (type) {
            case PlayerType::Fireboy:
                return std::make_unique<FireboyCharacter>(
                    "Fireboy", "assets/fireboy1.png", spawnPos, 3, sf::Color::Red);
            case PlayerType::Watergirl:
                return std::make_unique<WatergirlCharacter>(
                    "Watergirl", "assets/watergirl1.png", spawnPos, 3, sf::Color::Blue);
            case PlayerType::Earthboy:
                return std::make_unique<EarthboyCharacter>(
                    "Earthboy", "assets/earthboy.png", spawnPos, 3, sf::Color::Green);
            case PlayerType::Airgirl:
                return std::make_unique<AirgirlCharacter>(
                    "Airgirl", "assets/airgirl.png", spawnPos, 3, sf::Color(220,220,255));
            default:
                throw std::invalid_argument("Unknown PlayerType provided to CharacterFactory");
        }
    }
};

#endif // OOP_CHARACTERFACTORY_H
