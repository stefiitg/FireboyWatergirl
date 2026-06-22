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
    movingPlatforms = other.movingPlatforms;
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
    movingPlatforms = other.movingPlatforms;
    return *this;
}

Map::~Map() {
    // explicit
}



void Map::clear() {
    // set all tiles to Empty and clear moving platforms
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            grid[r][c] = Tile(TileType::Empty, c, r);
        }
    }
    movingPlatforms.clear();
}

void Map::loadLevel(LevelType level) {
    clear();
    switch (level) {
        case LevelType::Level1: generateLevel1(); break;
        case LevelType::Level2: generateLevel2(); break;
        case LevelType::Level3: generateLevel3(); break;
        case LevelType::Level4: generateLevel4(); break;
        default: generateLevel1(); break;
    }
}

void Map::generateLevel1() {
    // Original content of generateAscendingPlatforms
    // niste placi solide
    const std::pair<int,int> solids[] = {
        {1, 7}, {3, 7}, {9, 5}, {11, 4}
    };
    for (const auto& rc : solids) {
        int c = rc.first;
        int r = rc.second;
        if (r >= 0 && r < height && c >= 0 && c < width) {
            grid[r][c] = Tile(TileType::Solid, c, r);
        }
    }

    if (height > 6 && width > 8) {
        const float ts = static_cast<float>(Tile::getSize());
        int row = 6; int col = 7;
        sf::Vector2f startPos(col * ts, row * ts);
        float minX = 6 * ts;
        float maxX = 8 * ts;
        movingPlatforms.emplace_back(startPos, minX, maxX, 80.f, 1);
    }

    grid[height-2][2] = Tile(TileType::HalfFire, 2, height-2);
    grid[height-3][width-3] = Tile(TileType::HalfWater, width-3, height-3);

    if (height > 2 && width > 1) {
        grid[height-2][1] = Tile(TileType::HalfFire, 1, height-2);
    }

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
    if (height > 2) {
        grid[2][width-2] = Tile(TileType::Solid, width-2, 2);
    }
    if (height > 2 && width > 1) {
        grid[2][1] = Tile(TileType::ExitWater, 1, 2);
    }
    if (height > 2 && width > 4) {
        grid[2][4] = Tile(TileType::ExitEarth, 4, 2);
    }
    if (height > 2 && width > 10) {
        grid[2][10] = Tile(TileType::ExitAir, 10, 2);
    }

    if (height > 3) {
        grid[3][1] = Tile(TileType::Solid, 1, 3);
        if (width > 4) {
            grid[3][4] = Tile(TileType::Solid, 4, 3);
        }
    }
    if (height > 3 && width > 10) {
        grid[3][10] = Tile(TileType::Solid, 10, 3);
    }
    if (height > 5 && width > 2) {
        grid[5][2] = Tile(TileType::Solid, 2, 5);
    }

    if (height > 8) {
        int r = 8;
        if (width > 7) grid[r][7] = Tile(TileType::WaterCoin, 7, r);
        if (width > 8) grid[r][8] = Tile(TileType::WaterCoin, 8, r);
        if (width > 9) grid[r][9] = Tile(TileType::WaterCoin, 9, r);
        if (width > 1) grid[r][1] = Tile(TileType::FireCoin, 1, r);
        if (width > 2) grid[r][2] = Tile(TileType::FireCoin, 2, r);
    }

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

void Map::generateLevel2() {
    // Different layout: more solids on a different pattern, different coin/exits placement
    // base ground
    for (int c = 0; c < width; ++c) {
        grid[height-1][c] = Tile(TileType::Solid, c, height-1);
    }
    // some platforms
    if (height > 4) {
        for (int c = 2; c < std::min(width, 6); ++c) grid[height-4][c] = Tile(TileType::Solid, c, height-4);
        for (int c = std::max(0, width-6); c < width-2; ++c) grid[height-6][c] = Tile(TileType::Solid, c, height-6);
    }
    // moving platform in a different row/range
    if (height > 5 && width > 6) {
        const float ts = static_cast<float>(Tile::getSize());
        int row = height - 5; int col = 3;
        sf::Vector2f startPos(col * ts, row * ts);
        float minX = 2 * ts;
        float maxX = 5 * ts;
        movingPlatforms.emplace_back(startPos, minX, maxX, 90.f, 1);
    }
    // exits on top row corners
    if (height > 2) {
        grid[1][1] = Tile(TileType::ExitFire, 1, 1);
        grid[1][3] = Tile(TileType::ExitWater, 3, 1);
        grid[1][width-4] = Tile(TileType::ExitEarth, width-4, 1);
        grid[1][width-2] = Tile(TileType::ExitAir, width-2, 1);
        // supports under exits
        grid[2][1] = Tile(TileType::Solid, 1, 2);
        grid[2][3] = Tile(TileType::Solid, 3, 2);
        grid[2][width-4] = Tile(TileType::Solid, width-4, 2);
        grid[2][width-2] = Tile(TileType::Solid, width-2, 2);
        grid[6][6]= Tile(TileType::Solid, 6, 6);
        grid[7][1]= Tile(TileType::Solid, 1, 7);
        grid[7][3]= Tile(TileType::Solid, 3, 7);
        grid[6][12]= Tile(TileType::Solid, 12, 6);
        grid[6][10]= Tile(TileType::Solid, 10, 6);
    }
    // hazards
    if (height > 3) {
        grid[height-2][std::min(2, width-1)] = Tile(TileType::HalfFire, std::min(2, width-1), height-2);
        grid[height-3][std::max(0, width-3)] = Tile(TileType::HalfWater, std::max(0, width-3), height-3);
    }
    // coins
    if (height > 4) {
        grid[height-4][1] = Tile(TileType::FireCoin, 1, height-4);
        if (width > 8) grid[height-6][8] = Tile(TileType::WaterCoin, 8, height-6);
        if (width > 5) grid[height-5][5] = Tile(TileType::EarthCoin, 5, height-5);
    }
}

void Map::generateLevel4() {
    // Dense level with multiple top platforms, exits at bottom corners,
    // mid-map mixed hazards, scattered coins of different types, and a few moving platforms
    // 1) Base ground solid line
    for (int c = 0; c < width; ++c) {
        grid[height - 1][c] = Tile(TileType::Solid, c, height - 1);
    }

    // 2) Exits at bottom corners with small supports
    if (width >= 2) {
        grid[height - 2][0] = Tile(TileType::ExitFire, 0, height - 2);
        grid[height - 2][width - 1] = Tile(TileType::ExitWater, width - 1, height - 2);
        // place other two exits near corners too
        if (width > 3) grid[height - 2][2] = Tile(TileType::ExitEarth, 2, height - 2);
        if (width > 4) grid[height - 2][width - 3] = Tile(TileType::ExitAir, width - 3, height - 2);
    }

    // 3) Top dense platforms
    int topRow1 = 1;
    int topRow2 = 2;
    for (int c = 1; c < width - 1; ++c) {
        if (c % 2 == 0) grid[topRow1][c] = Tile(TileType::Solid, c, topRow1);
        if (c % 3 != 0) grid[topRow2][c] = Tile(TileType::Solid, c, topRow2);
    }

    // 4) Mid platforms in a staggered pattern

   int midStart = std::max(4, height / 2 - 1);
    for (int r = midStart; r < height - 2; r += 2) {
        for (int c = 1; c < width - 1; ++c) {
            if (((r + c) % 3) == 0) grid[r][c] = Tile(TileType::Solid, c, r);
        }
    }
   /*int  midStart=height/2+2;
   int r=midStart;
    for (int c=1;c<width;c++)
    if (c%3==1) grid[r][c]=Tile(TileType::Solid, c, r);*/

grid[6][1]=Tile(TileType::Solid, 1, 6);
    // 5) Mixed hazards in the middle: half-fire and half-water in alternating columns
    int hazardRow = height / 2;
    for (int c = 2; c < width - 2; ++c) {
        if (c % 2 == 0)
            grid[hazardRow][c] = Tile(TileType::HalfFire, c, hazardRow);
        else
            grid[hazardRow][c] = Tile(TileType::HalfWater, c, hazardRow);
    }

    // 6) Coins of various types scattered
    // Ensure at least 4 coins of different types
    if (width > 2 && height > 4) {
        grid[3][2] = Tile(TileType::FireCoin, 2, 3);
        grid[6][width - 2] = Tile(TileType::WaterCoin, width - 2, 6);
        grid[6][2] = Tile(TileType::EarthCoin, 2, 6);

    }
    // additional scattered coins


    // 7) A couple of moving platforms near the center and upper mid
    const float ts = static_cast<float>(Tile::getSize());
    if (width > 6 && height > 6) {
        int row = height / 2 - 1; int col = width / 2 - 2;
        sf::Vector2f startPos(col * ts, row * ts);
        float minX = (col - 2) * ts;
        float maxX = (col + 2) * ts;
        movingPlatforms.emplace_back(startPos, minX, maxX, 70.f, 1);
    }
    if (width > 10 && height > 4) {
        int row = 4; int col = width / 2;
        sf::Vector2f startPos(col * ts, row * ts);
        float minX = (col - 3) * ts;
        float maxX = (col + 3) * ts;
        movingPlatforms.emplace_back(startPos, minX, maxX, 90.f, 1);
    }
}

void Map::generateLevel3() {
  const std::pair<int,int> solids[] = {
        {1, 7}, {3, 7}, {9, 5}, {11, 4}
    };
    for (const auto& rc : solids) {
        int c = rc.first;
        int r = rc.second;
        if (r >= 0 && r < height && c >= 0 && c < width) {
            grid[r][c] = Tile(TileType::Solid, c, r);
        }
    }

    if (height > 6 && width > 8) {
        const float ts = static_cast<float>(Tile::getSize());
        int row = 6; int col = 7;
        sf::Vector2f startPos(col * ts, row * ts);
        float minX = 6 * ts;
        float maxX = 8 * ts;
        movingPlatforms.emplace_back(startPos, minX, maxX, 80.f, 1);
    }

    grid[height-2][2] = Tile(TileType::HalfFire, 2, height-2);
    grid[height-3][width-3] = Tile(TileType::HalfWater, width-3, height-3);

    if (height > 2 && width > 1) {
        grid[height-2][1] = Tile(TileType::HalfFire, 1, height-2);
    }

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
    if (height > 2) {
        grid[2][width-2] = Tile(TileType::Solid, width-2, 2);
    }
    if (height > 2 && width > 1) {
        grid[2][1] = Tile(TileType::ExitWater, 1, 2);
    }
    if (height > 2 && width > 4) {
        grid[2][4] = Tile(TileType::ExitEarth, 4, 2);
    }
    if (height > 2 && width > 10) {
        grid[2][10] = Tile(TileType::ExitAir, 10, 2);
    }

    if (height > 3) {
        grid[3][1] = Tile(TileType::Solid, 1, 3);
        if (width > 4) {
            grid[3][4] = Tile(TileType::Solid, 4, 3);
        }
    }
    if (height > 3 && width > 10) {
        grid[3][10] = Tile(TileType::Solid, 10, 3);
    }
    if (height > 5 && width > 2) {
        grid[5][2] = Tile(TileType::Solid, 2, 5);
    }

    if (height > 8) {
        int r = 8;
        if (width > 7) grid[r][7] = Tile(TileType::WaterCoin, 7, r);
        if (width > 8) grid[r][8] = Tile(TileType::WaterCoin, 8, r);
        if (width > 9) grid[r][9] = Tile(TileType::WaterCoin, 9, r);
        if (width > 1) grid[r][1] = Tile(TileType::FireCoin, 1, r);
        if (width > 2) grid[r][2] = Tile(TileType::FireCoin, 2, r);
    }

    if (height > 5 && width > 7) {
        grid[5][7] = Tile(TileType::EarthCoin, 7, 5);
    }
    if (height > 4 && width > 9) {
        grid[4][9] = Tile(TileType::EarthCoin, 9, 4);
    }
    if (height > 3 && width > 11) {
        grid[3][11] = Tile(TileType::EarthCoin, 11, 3);
    }

    // adding la lvl:
    if (height > 4 && width > 6) {
        grid[4][6] = Tile(TileType::Solid, 6, 4);
    }
    if (height > 8 && width > 3) {
        grid[8][3] = Tile(TileType::FireCoin, 3, 8);
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

    for (const auto& mp : movingPlatforms) mp.draw(target);
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
    return sf::Vector2f(Tile::getSize() * 4.f, Tile::getSize() * (height - 2));
}

sf::Vector2f Map::respawnWorldPosForWater() const {
    return sf::Vector2f(Tile::getSize() * 5.f, Tile::getSize() * (height - 2));
}

sf::Vector2f Map::respawnWorldPosForEarth() const {

    return sf::Vector2f(Tile::getSize() * 6.f, Tile::getSize() * (height - 2));
}

sf::Vector2f Map::respawnWorldPosForAir() const {
    // Fixed spawn at (row=8, col=4)
    return sf::Vector2f(Tile::getSize() * 7.f, Tile::getSize() * (height-2));
}

void Map::update(float dt) {
    for (auto& mp : movingPlatforms) mp.update(dt);
}
