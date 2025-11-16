#ifndef OOP_TILE_H
#define OOP_TILE_H

#include <SFML/Graphics.hpp>
#include <ostream>
#include <string>

enum class TileType { Empty, Solid, Fire, Water, ExitFire, ExitWater };

// Add toString declaration
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
    TileType type_ = TileType::Empty;
    [[maybe_unused]] int col_ = 0;
    [[maybe_unused]] int row_ = 0;
    sf::RectangleShape shape_;
};

#endif // OOP_TILE_H