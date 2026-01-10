#ifndef OOP_CHARACTER_H
#define OOP_CHARACTER_H

#include <SFML/Graphics.hpp>
#include <memory>
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

    // "Constructor" virtual (clone) pentru copiere polimorfă
    virtual std::unique_ptr<Character> clone() const = 0;

    // Interacțiuni polimorfice cu tile-urile (evită if-uri pe tipuri în Game)
    // Implicit, un personaj neutru: doar Solid este solid, Fire/Water sunt letale,
    // niciun exit nu este potrivit; Half-tiles partea de sus este letală pentru ambele.
    virtual bool isSolidOn(TileType tt) const {
        return tt == TileType::Solid; // implicit doar Solid este suport
    }
    virtual bool isDeadlyOn(TileType tt) const {
        return tt == TileType::Fire || tt == TileType::Water; // implicit ambele omoară
    }
    // nu folosim parametrul in implementarea de baza; omitem numele pentru a evita -Werror=unused-parameter
    virtual bool canExitThrough(TileType) const { return false; }
    virtual bool isTopHalfDeadly(TileType tt) const {
        return tt == TileType::HalfFire || tt == TileType::HalfWater;
    }

    // rule of 3 (explicit copy ops for the exercise)
    Character(const Character& other);
    Character& operator=(const Character& other);

    // state helpers
    void setOnGround(bool state);

    // getters (pot fi neutilizate în unele build-uri CI)
    [[maybe_unused]] const std::string& getName() const { return name; }
    [[maybe_unused]] int getLives() const { return lives; }
    [[maybe_unused]] sf::Vector2f getPosition() const { return position; }

    // expose bounds for collision checks
    sf::FloatRect bounds() const;

    // position
    void setPosition(const sf::Vector2f& p);

    // behavior
    bool update(float dt, const sf::FloatRect& worldBounds);
    void moveLeft(float dt);
    void moveRight(float dt);
    void jump();
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
    std::unique_ptr<Character> clone() const override {
        return std::make_unique<FireboyCharacter>(*this);
    }
    bool isSolidOn(TileType tt) const override {
        return tt == TileType::Solid || tt == TileType::Fire;
    }
    bool isDeadlyOn(TileType tt) const override {
        return tt == TileType::Water; // apa omoară focul
    }
    bool canExitThrough(TileType tt) const override { return tt == TileType::ExitFire; }
    bool isTopHalfDeadly(TileType tt) const override { return tt == TileType::HalfWater; }
};

class WatergirlCharacter : public Character {
public:
    using Character::Character;
    Element element() const override { return Element::Water; }
    std::unique_ptr<Character> clone() const override {
        return std::make_unique<WatergirlCharacter>(*this);
    }
    bool isSolidOn(TileType tt) const override {
        return tt == TileType::Solid || tt == TileType::Water;
    }
    bool isDeadlyOn(TileType tt) const override {
        return tt == TileType::Fire; // focul omoară apa
    }
    bool canExitThrough(TileType tt) const override { return tt == TileType::ExitWater; }
    bool isTopHalfDeadly(TileType tt) const override { return tt == TileType::HalfFire; }
};

class RockCharacter : public Character {
public:
    using Character::Character;
    Element element() const override { return Element::Neutral; }
    std::unique_ptr<Character> clone() const override {
        return std::make_unique<RockCharacter>(*this);
    }
    // pentru rock: doar Solid e suport, ambele lichide sunt letale, nu are exit dedicat,
    // ambele jumătăți superioare sunt letale
};

#endif // OOP_CHARACTER_H
