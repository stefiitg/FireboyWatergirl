#include "Map.h"
#include <random>
#include <algorithm>

void Map::allocateGrid(int w, int h, TileType defaultType) {
    width = w; height = h;
    grid.assign(height, std::vector<Tile>(width, Tile(defaultType, 0, 0)));
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c)
            grid[r][c] = Tile(defaultType, c, r);
}

Map::Map(int w, int h, TileType defaultType) {
    allocateGrid(w,h,defaultType);
}

Map::Map(const Map& other) {
    width = other.width;
    height = other.height;
    grid.reserve(height);
    for (int r = 0; r < height; ++r) {
        std::vector<Tile> row;
        row.reserve(width);
        for (int c = 0; c < width; ++c) row.push_back(other.grid[r][c]);
        grid.push_back(std::move(row));
    }
}

Map& Map::operator=(const Map& other) {
    if (this == &other) return *this;
    width = other.width;
    height = other.height;
    grid.clear();
    grid.reserve(height);
    for (int r = 0; r < height; ++r) {
        std::vector<Tile> row;
        row.reserve(width);
        for (int c = 0; c < width; ++c) row.push_back(other.grid[r][c]);
        grid.push_back(std::move(row));
    }
    return *this;
}

Map::~Map() {
    // explicit for the exercise; nothing special to release
}

void Map::generateAscendingPlatforms(unsigned seed) {
    for (int r=0;r<height;++r)
        for (int c=0;c<width;++c)
            grid[r][c] = Tile(TileType::Empty, c, r);

    std::mt19937 rng((seed==0)? std::random_device{}() : seed);
    std::uniform_int_distribution<int> gapDist(1,3);
    std::uniform_int_distribution<int> lengthDist(1,3);

    int currentRow = height - 2;
    int c = 1;
    while (c < width-1 && currentRow > 0) {
        int len = lengthDist(rng);
        for (int k = 0; k < len && c < width-1; ++k) {
            grid[currentRow][c] = Tile(TileType::Solid, c, currentRow);
            ++c;
        }
        int gap = gapDist(rng);
        c += gap;
        if (currentRow > 1) currentRow -= 1;
    }

    grid[height-2][2] = Tile(TileType::Fire, 2, height-2);
    grid[height-3][width-3] = Tile(TileType::Water, width-3, height-3);
    grid[1][width-2] = Tile(TileType::ExitFire, width-2, 1);
    grid[1][1] = Tile(TileType::ExitWater, 1, 1);
    if (height > 3) {
        grid[3][1] = Tile(TileType::Solid, 1, 3);
    }
    if (height > 5 && width > 2) {
        grid[5][2] = Tile(TileType::Solid, 2, 5);
    }
}

TileType Map::getTileTypeAtGrid(int col, int row) const {
    if (col < 0 || col >= width || row < 0 || row >= height) return TileType::Solid;
    return grid[row][col].getType();
}

TileType Map::getTileTypeAtWorld(float x, float y) const {
    int col = static_cast<int>(x / Tile::getSize());
    int row = static_cast<int>(y / Tile::getSize());
    return getTileTypeAtGrid(col, row);
}

void Map::draw(sf::RenderTarget& target) const {
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c)
            grid[r][c].draw(target);
}

std::ostream& operator<<(std::ostream& os, const Map& m) {
    os << "Map " << m.width << "x" << m.height << "\n";
    for (int r = 0; r < m.height; ++r) {
        for (int c = 0; c < m.width; ++c) {
            os << m.grid[r][c];
        }
        os << "\n";
    }
    return os;
}

sf::FloatRect Map::worldBounds() const {
    return sf::FloatRect(0.f, 0.f, width * Tile::getSize(), height * Tile::getSize());
}

sf::Vector2f Map::respawnWorldPosForFire() const {
    return sf::Vector2f(Tile::getSize() * 1.f, Tile::getSize() * (height - 2));
}

sf::Vector2f Map::respawnWorldPosForWater() const {
    return sf::Vector2f(Tile::getSize() * 5.f, Tile::getSize() * (height - 2));
}
