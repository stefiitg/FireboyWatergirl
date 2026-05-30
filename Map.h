#ifndef OOP_MAP_H
#define OOP_MAP_H

#include <vector>
#include <ostream>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "MovingPlatform.h"

// Levels available in the game
enum class LevelType {
    Level1,
    Level2,
    Level3
};

class Map {
private:
    std::vector<std::vector<Tile>> grid;
    int width{}, height{};
    std::vector<MovingPlatform> movingPlatforms;

    void allocateGrid(int w, int h, TileType defaultType = TileType::Empty);
    void generateLevel1();
    void generateLevel2();
    void generateLevel3();
    void clear();

public:
    explicit Map(int w = 12, int h = 8, TileType defaultType = TileType::Empty);
    Map(const Map& other);
    Map& operator=(const Map& other);
    ~Map();

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Legacy generator kept for backward-compat; Level1 is the original layout
    void generateAscendingPlatforms(unsigned seed = 0);
    // Load a specific level layout
    void loadLevel(LevelType level);
    void update(float dt);

    TileType getTileTypeAtGrid(int col, int row) const;
    [[maybe_unused]] TileType getTileTypeAtWorld(float x, float y) const;

    // setter util pentru a modifica un tile in timpul jocului (ex: colectare moneda)
    void setTileTypeAtGrid(int col, int row, TileType t);

    void draw(sf::RenderTarget& target) const;
    friend std::ostream& operator<<(std::ostream& os, const Map& m);

    sf::FloatRect worldBounds() const;

    sf::Vector2f respawnWorldPosForFire() const;
    sf::Vector2f respawnWorldPosForWater() const;
    sf::Vector2f respawnWorldPosForEarth() const;
    sf::Vector2f respawnWorldPosForAir() const;

    const std::vector<MovingPlatform>& getMovingPlatforms() const { return movingPlatforms; }
};

#endif // OOP_MAP_H
