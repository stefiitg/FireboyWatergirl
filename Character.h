#ifndef OOP_CHARACTER_H
#define OOP_CHARACTER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <ostream>
#include "Tile.h"

// element affinity used for gameplay rules
enum class Element { Fire, Water, Neutral };

class Character {
private:
    std::string name;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape fallbackShape;
    bool usingTexture{};
    sf::Vector2f position{}; // world coordinates (top-left)
    sf::Vector2f velocity{};
    int lives{0};
    bool onGround{false};

    float speed = 160.f; // px/s
    float jumpImpulse = 360.f; // initial jump velocity
    static constexpr float GRAVITY = 900.f;

    void initFallbackShape(const sf::Color& c, const sf::Vector2f& size);

public:
    Character(const std::string& nm, const std::string& texturePath,
              const sf::Vector2f& pos = {0.f,0.f}, int lifeCount = 3,
              const sf::Color& fallbackColor = sf::Color::White);
    virtual ~Character();

    // polymorphic attribute specific to the theme (used for gameplay rules)
    virtual Element element() const = 0;

    // rule of 3 (explicit copy ops for the exercise)
    Character(const Character& other);
    Character& operator=(const Character& other);

    // state helpers
    void setOnGround(bool state);

    // getters
    const std::string& getName() const { return name; }
    int getLives() const { return lives; }
    sf::Vector2f getPosition() const { return position; }

    // expose bounds for collision checks
    sf::FloatRect bounds() const;

    // position
    void setPosition(const sf::Vector2f& p);

    // behavior
    bool update(float dt, const sf::FloatRect& worldBounds);
    void moveLeft(float dt);
    void moveRight(float dt);
    void jump();
    void takeDamageAndRespawn(const sf::Vector2f& respawnPos);
    void draw(sf::RenderTarget& target) const;
    void setFallbackAppearance(const sf::Color& c);
    void stopVerticalMovement();

    friend std::ostream& operator<<(std::ostream& os, const Character& c);
};

// Derived characters
class FireboyCharacter : public Character {
public:
    using Character::Character;
    Element element() const override { return Element::Fire; }
};

class WatergirlCharacter : public Character {
public:
    using Character::Character;
    Element element() const override { return Element::Water; }
};

class RockCharacter : public Character {
public:
    using Character::Character;
    Element element() const override { return Element::Neutral; }
};

#endif // OOP_CHARACTER_H
