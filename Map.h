#ifndef OOP_MAP_H
#define OOP_MAP_H

#include <vector>
#include <ostream>
#include <SFML/Graphics.hpp>
#include "Tile.h"

class Map {
private:
    std::vector<std::vector<Tile>> grid;
    int width{}, height{};

    void allocateGrid(int w, int h, TileType defaultType = TileType::Empty);

public:
    explicit Map(int w = 12, int h = 8, TileType defaultType = TileType::Empty);
    Map(const Map& other);
    Map& operator=(const Map& other);
    ~Map();

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void generateAscendingPlatforms(unsigned seed = 0);

    TileType getTileTypeAtGrid(int col, int row) const;
    TileType getTileTypeAtWorld(float x, float y) const;

    void draw(sf::RenderTarget& target) const;
    friend std::ostream& operator<<(std::ostream& os, const Map& m);

    sf::FloatRect worldBounds() const;

    sf::Vector2f respawnWorldPosForFire() const;
    sf::Vector2f respawnWorldPosForWater() const;
};

#endif // OOP_MAP_H
