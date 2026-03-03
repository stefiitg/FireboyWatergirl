#include "Map.h"
#include "GameExceptions.h"
#include <algorithm>

void Map::allocateGrid(int w, int h, TileType defaultType) {
    width = w; height = h;
    grid.assign(height, std::vector<Tile>(width, Tile(defaultType, 0, 0)));
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c)
            grid[r][c] = Tile(defaultType, c, r);
}

Map::Map(int w, int h, TileType defaultType) {
    if (w <= 0 || h <= 0) {
        throw InvalidMapError("Map dimensions must be positive");
    }
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
    // explicit
}

void Map::generateAscendingPlatforms(unsigned seed) {
    //  generarea aleatoare -eliminata pentru moment
    (void)seed;

    // resetare harta la empty
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c)
            grid[r][c] = Tile(TileType::Empty, c, r);

    // niste placi solide
    const std::pair<int,int> solids[] = {
        {1, 7}, {3, 7}, {7, 6}, {9, 5}, {11, 4}
    };
    for (const auto& rc : solids) {
        int c = rc.first;
        int r = rc.second;
        if (r >= 0 && r < height && c >= 0 && c < width) {
            grid[r][c] = Tile(TileType::Solid, c, r);
        }
    }

    //  (row:7,col:2) -> halfFire
    //  (row:6,col:11) - > halfWater
    grid[height-2][2] = Tile(TileType::HalfFire, 2, height-2);
    grid[height-3][width-3] = Tile(TileType::HalfWater, width-3, height-3);

    //  (row:7,col:1) -- halfFire
    if (height > 2 && width > 1) {
        grid[height-2][1] = Tile(TileType::HalfFire, 1, height-2);
    }

    // Solid: (row:7,col:0), (row:6,col:10), (row:6,col:12)
    if (height > 2 && width > 0) {
        grid[height-2][0] = Tile(TileType::Solid, 0, height-2);
    }
    if (height > 3 && width > 10) {
        grid[height-3][10] = Tile(TileType::Solid, 10, height-3);
    }
    if (height > 3 && width > 12) {
        grid[height-3][12] = Tile(TileType::Solid, 12, height-3);
    }
    grid[1][width-2] = Tile(TileType::ExitFire, width-2, 1);
    // tile solid pt suportul lui fireboy la exit
    if (height > 2) {
        grid[2][width-2] = Tile(TileType::Solid, width-2, 2);
    }
    //  watergirl exit-- (row=2, col=1)
    if (height > 2 && width > 1) {
        grid[2][1] = Tile(TileType::ExitWater, 1, 2);
    }
    // Earthboy exit-- (row:2 , col:4)
    if (height > 2 && width > 4) {
        grid[2][4] = Tile(TileType::ExitEarth, 4, 2);
    }

    if (height > 3) {
        grid[3][1] = Tile(TileType::Solid, 1, 3);
        if (width > 4) {
            grid[3][4] = Tile(TileType::Solid, 4, 3);
        }
    }
    if (height > 5 && width > 2) {
        grid[5][2] = Tile(TileType::Solid, 2, 5);
    }

    // monede specifice personajelor:
    // watergirl: (row:8,col:7),(8,8),(8,9)
    // fireboy: (row:8,col:1),(8,2)
    if (height > 8) {
        int r = 8;
        if (width > 7) grid[r][7] = Tile(TileType::WaterCoin, 7, r);
        if (width > 8) grid[r][8] = Tile(TileType::WaterCoin, 8, r);
        if (width > 9) grid[r][9] = Tile(TileType::WaterCoin, 9, r);
        if (width > 1) grid[r][1] = Tile(TileType::FireCoin, 1, r);
        if (width > 2) grid[r][2] = Tile(TileType::FireCoin, 2, r);
    }

    // Earthboy coins  (row,col): (5,7), (4,9), (3,11)
    // ne asiguram ca dimensiunile mapei permit coinurile
    if (height > 5 && width > 7) {
        grid[5][7] = Tile(TileType::EarthCoin, 7, 5);
    }
    if (height > 4 && width > 9) {
        grid[4][9] = Tile(TileType::EarthCoin, 9, 4);
    }
    if (height > 3 && width > 11) {
        grid[3][11] = Tile(TileType::EarthCoin, 11, 3);
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

void Map::setTileTypeAtGrid(int col, int row, TileType t) {
    if (col < 0 || col >= width || row < 0 || row >= height) return;
    grid[row][col] = Tile(t, col, row);
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

//pozitiile de spawn ptr personaje
sf::Vector2f Map::respawnWorldPosForFire() const {
    return sf::Vector2f(Tile::getSize() * 1.f, Tile::getSize() * (height - 2));
}

sf::Vector2f Map::respawnWorldPosForWater() const {
    return sf::Vector2f(Tile::getSize() * 5.f, Tile::getSize() * (height - 2));
}

sf::Vector2f Map::respawnWorldPosForEarth() const {

    return sf::Vector2f(Tile::getSize() * 3.f, Tile::getSize() * (height - 2));
}
