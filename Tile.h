#ifndef OOP_TILE_H
#define OOP_TILE_H

#include <SFML/Graphics.hpp>
#include <ostream>
#include <string>

enum class TileType { Empty, Solid, Fire, Water, HalfFire, HalfWater, Coin, FireCoin, WaterCoin, EarthCoin, ExitFire, ExitWater, ExitEarth, ExitAir };


std::string toString(TileType t);

class Tile {
public:
    Tile() = default;
    Tile(TileType t, int col, int row);

    TileType getType() const { return type_; }

//change
    void draw(sf::RenderTarget& target) const;
    static constexpr int getSize() { return 48; }

    friend std::ostream& operator<<(std::ostream& os, const Tile& t);

private:
    // texturi statice ptr tile ul solid
    static sf::Texture solidTex;
    static bool solidTexLoaded;
    static bool solidTexOk;      // -daca incarcarea e cu succes

    // texturi statice ptr tile urile exit
    static sf::Texture exitFireTex;
    static sf::Texture exitWaterTex;
    static sf::Texture exitEarthTex;
    static sf::Texture exitAirTex;
    static bool exitTexturesLoaded;
    static bool exitFireLoaded;          // flag uri individuale pt incarcare cu succes
    static bool exitWaterLoaded;
    static bool exitEarthLoaded;
    static bool exitAirLoaded;

    static void ensureExitTexturesLoaded();
    static void ensureSolidTextureLoaded();

    TileType type_ = TileType::Empty;
    [[maybe_unused]] int col_ = 0;
    [[maybe_unused]] int row_ = 0;
    sf::RectangleShape shape_;
};

#endif // OOP_TILE_H