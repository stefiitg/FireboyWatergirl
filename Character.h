#ifndef OOP_CHARACTER_H
#define OOP_CHARACTER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <utility>
#include <string>
#include <ostream>
#include "Tile.h"


enum class Element { Fire, Water, Neutral, Air };

class Character {
private:
    std::string name;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape fallbackShape;
    bool usingTexture{};
    sf::Vector2f position{}; // coord globale (top-left)
    sf::Vector2f velocity{};
    int lives{0};
    bool onGround{false};

    float speed = 160.f; // px/s
    float jumpImpulse = 360.f;
    static constexpr float GRAVITY = 900.f;

    void initFallbackShape(const sf::Color& c, const sf::Vector2f& size);

public:
    Character(const std::string& nm, const std::string& texturePath,
              const sf::Vector2f& pos = {0.f,0.f}, int lifeCount = 3,
              const sf::Color& fallbackColor = sf::Color::White);
    virtual ~Character();

    // atribut polimorfic
    virtual Element element() const = 0;

    // constructor virtual pentru copiere polimorfa
    virtual std::unique_ptr<Character> clone() const = 0;

    // interacțiuni polimorfice cu tile urile (evita if uri pe tipuri in game)
    // implicit , un personaj neutru : doar Solid e solid, fire/water sunt letale
    // by default niciun exit nu este potrivit, la half tiles partea de sus este letala pentru ambele
    virtual bool isSolidOn(TileType tt) const {
        return tt == TileType::Solid;
    }
    virtual bool isDeadlyOn(TileType tt) const {
        return tt == TileType::Fire || tt == TileType::Water; // implicit ambele omoară
    }

    virtual bool canExitThrough(TileType) const { return false; }
    virtual bool isTopHalfDeadly(TileType tt) const {
        return tt == TileType::HalfFire || tt == TileType::HalfWater;
    }

    // rule of 3
    Character(const Character& other);
    // clasa este abstracta
    // folosim o varianta clasică prin const&
    Character& operator=(const Character& other);


    void swap(Character& other) noexcept;


    void setOnGround(bool state);

    // getters -pot fi neutilizati in unele build uri )
    [[maybe_unused]] const std::string& getName() const { return name; }
    [[maybe_unused]] int getLives() const { return lives; }
    [[maybe_unused]] sf::Vector2f getPosition() const { return position; }


    sf::FloatRect bounds() const;


    void setPosition(const sf::Vector2f& p);

    // comportament
    bool update(float dt, const sf::FloatRect& worldBounds);
    void moveLeft(float dt);
    void moveRight(float dt);
    void jump();
    void draw(sf::RenderTarget& target) const;

    void setFallbackAppearance();
    void stopVerticalMovement();


    bool isUsingTexture() const { return usingTexture; }

    // afisare virtuala cu operator<<
    virtual void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const Character& c);
protected:
    // NVI pe randare: draw() public non-virtual va apela acest hook virtual
    virtual void drawImpl(sf::RenderTarget& target) const;
    // Each derived character specifies its own default fallback color (polymorphic)
    virtual sf::Color getDefaultFallbackColor() const = 0;
};

// derivate
class FireboyCharacter : public Character {
public:
    // constrctor explicit care apelează ctorul bazei
    FireboyCharacter(const std::string& nm,
                     const std::string& texturePath,
                     const sf::Vector2f& pos = {0.f, 0.f},
                     int lifeCount = 3,
                     const sf::Color& fallbackColor = sf::Color::White)
        : Character(nm, texturePath, pos, lifeCount, fallbackColor) {}
    Element element() const override { return Element::Fire; }
    std::unique_ptr<Character> clone() const override {
        return std::make_unique<FireboyCharacter>(*this);
    }
    void print(std::ostream& os) const override {
        Character::print(os);
        os << " element=Fire";
    }
    bool isSolidOn(TileType tt) const override {
        return tt == TileType::Solid || tt == TileType::Fire;
    }
    bool isDeadlyOn(TileType tt) const override {
        return tt == TileType::Water; // apa omoara focul
    }
    bool canExitThrough(TileType tt) const override { return tt == TileType::ExitFire; }
    bool isTopHalfDeadly(TileType tt) const override { return tt == TileType::HalfWater; }
protected:
    sf::Color getDefaultFallbackColor() const override { return sf::Color::Red; }
};

class WatergirlCharacter : public Character {
public:
    // ctor explicit care apeleaza ctorul bazei în lista de inițializare
    WatergirlCharacter(const std::string& nm,
                       const std::string& texturePath,
                       const sf::Vector2f& pos = {0.f, 0.f},
                       int lifeCount = 3,
                       const sf::Color& fallbackColor = sf::Color::White)
        : Character(nm, texturePath, pos, lifeCount, fallbackColor) {}
    Element element() const override { return Element::Water; }
    std::unique_ptr<Character> clone() const override {
        return std::make_unique<WatergirlCharacter>(*this);
    }
    void print(std::ostream& os) const override {
        Character::print(os);
        os << " element=Water";
    }
    bool isSolidOn(TileType tt) const override {
        return tt == TileType::Solid || tt == TileType::Water;
    }
    bool isDeadlyOn(TileType tt) const override {
        return tt == TileType::Fire; // focul omoara apa
    }
    bool canExitThrough(TileType tt) const override { return tt == TileType::ExitWater; }
    bool isTopHalfDeadly(TileType tt) const override { return tt == TileType::HalfFire; }
protected:
    sf::Color getDefaultFallbackColor() const override { return sf::Color::Blue; }
};



// Earthboy
class EarthboyCharacter : public Character {
public:
    EarthboyCharacter(const std::string& nm,
                      const std::string& texturePath,
                      const sf::Vector2f& pos = {0.f, 0.f},
                      int lifeCount = 3,
                      const sf::Color& fallbackColor = sf::Color::Green)
        : Character(nm, texturePath, pos, lifeCount, fallbackColor) {}
    Element element() const override { return Element::Neutral; }
    std::unique_ptr<Character> clone() const override {
        return std::make_unique<EarthboyCharacter>(*this);
    }
    void print(std::ostream& os) const override {
        Character::print(os);
        os << " element=Neutral(Earth)";
    }
    // comportament neutru: doar solid e suport, lichidele omoara (moștenit)
    bool canExitThrough(TileType tt) const override { return tt == TileType::ExitEarth; }
protected:
    sf::Color getDefaultFallbackColor() const override { return sf::Color::Green; }
};

// Airgirl e clasa derivata adaugata la final din baza character:
class AirgirlCharacter : public Character {
public:
    AirgirlCharacter(const std::string& nm,
                     const std::string& texturePath,
                     const sf::Vector2f& pos = {0.f, 0.f},
                     int lifeCount = 3,
                     const sf::Color& fallbackColor = sf::Color(220,220,255))
        : Character(nm, texturePath, pos, lifeCount, fallbackColor) {}
    Element element() const override { return Element::Air; }
    std::unique_ptr<Character> clone() const override {
        return std::make_unique<AirgirlCharacter>(*this);
    }
    void print(std::ostream& os) const override {
        Character::print(os);
        os << " element=Air";
    }
    bool canExitThrough(TileType tt) const override { return tt == TileType::ExitAir; }
protected:
    sf::Color getDefaultFallbackColor() const override { return sf::Color::White; }
};

#endif

