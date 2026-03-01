#ifndef OOP_TILE_H
#define OOP_TILE_H

#include <SFML/Graphics.hpp>
#include <ostream>
#include <string>

enum class TileType { Empty, Solid, Fire, Water, HalfFire, HalfWater, Coin, FireCoin, WaterCoin, EarthCoin, ExitFire, ExitWater, ExitEarth };

//add to string
std::string toString(TileType t);

class Tile {
public:
    Tile() = default;
    Tile(TileType t, int col, int row);

    TileType getType() const { return type_; }
    //void setType(TileType t) { type_ = t; }
//change
    void draw(sf::RenderTarget& target) const;
    static constexpr int getSize() { return 48; }

    friend std::ostream& operator<<(std::ostream& os, const Tile& t);

private:
    // Static texture and flags for Solid tile (loaded once on demand)
    static sf::Texture solidTex;
    static bool solidTexLoaded;  // guard to attempt loading only once
    static bool solidTexOk;      // whether loading succeeded

    // Static textures for exit tiles (loaded once on demand)
    static sf::Texture exitFireTex;
    static sf::Texture exitWaterTex;
    static sf::Texture exitEarthTex;
    static bool exitTexturesLoaded;      // guard to attempt loading only once
    static bool exitFireLoaded;          // individual success flags
    static bool exitWaterLoaded;
    static bool exitEarthLoaded;

    static void ensureExitTexturesLoaded();
    static void ensureSolidTextureLoaded();

    TileType type_ = TileType::Empty;
    [[maybe_unused]] int col_ = 0;
    [[maybe_unused]] int row_ = 0;
    sf::RectangleShape shape_;
};

#endif // OOP_TILE_H