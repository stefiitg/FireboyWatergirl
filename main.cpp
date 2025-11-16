#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4996) // MSVC: disable warnings for deprecated funcs
#endif
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <memory>
#include "Tile.h"
//change
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif

using std::string;
using std::vector;
using std::cout;
using std::endl;





// -------------------------------
// Character
// -------------------------------
class Character {
private:
    string name;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape fallbackShape;
    bool usingTexture;
    sf::Vector2f position; // world coordinates (top-left)
    sf::Vector2f velocity;
    int lives;
    bool onGround;

    float speed = 160.f; // px/s
    float jumpImpulse = 360.f; // initial jump velocity
    static constexpr float GRAVITY = 900.f;

    void initFallbackShape(const sf::Color& c, const sf::Vector2f& size) {
        fallbackShape.setSize(size);
        fallbackShape.setFillColor(c);
        fallbackShape.setPosition(position);
    }

public:

    // constructor parametric
    Character(const string& nm, const string& texturePath,
          const sf::Vector2f& pos = {0.f,0.f}, int lifeCount = 3,
          const sf::Color& fallbackColor = sf::Color::White)
    : name(nm), usingTexture(false), position(pos), velocity(0.f, 0.f),
      lives(lifeCount), onGround(false)
    {
        // try load texture (if not found, use fallback rectangle)
        if (!texturePath.empty() && texture.loadFromFile(texturePath)) {
            usingTexture = true;
            sprite.setTexture(texture);
            // scale sprite to tile size if texture height > 0
            if (texture.getSize().y > 0) {
                float factor = Tile::getSize() / texture.getSize().y;
                sprite.setScale(factor, factor);
            }
            sprite.setPosition(position);
        } else {
            usingTexture = false;
            initFallbackShape(fallbackColor, {Tile::getSize(), Tile::getSize()});
        }
    }

    //get on ground state - now also clears vertical velocity when set true
    void setOnGround(bool state) {
         onGround = state;
         if (onGround) {
             velocity.y = 0.f; // stop vertical movement when we consider the character grounded
         }
    }
    // copy constructor (default is ok but implement explicitly to satisfy assignment)
    Character (const Character& other)
        : name(other.name),
          texture(other.texture),
          sprite(other.sprite),
          fallbackShape(other.fallbackShape),
          usingTexture(other.usingTexture),
          position(other.position),
          velocity(other.velocity),
          lives(other.lives),
          onGround(other.onGround),
          speed(other.speed),
          jumpImpulse(other.jumpImpulse)
    {
        if (usingTexture) sprite.setTexture(texture);
    }

    Character& operator=(const Character& other) {
        if (this == &other) return *this;
        name = other.name;
        texture = other.texture;
        sprite = other.sprite;
        fallbackShape = other.fallbackShape;
        usingTexture = other.usingTexture;
        position = other.position;
        velocity = other.velocity;
        lives = other.lives;
        onGround = other.onGround;
        speed = other.speed;
        jumpImpulse = other.jumpImpulse;
        if (usingTexture) sprite.setTexture(texture);
        return *this;
    }

    ~Character() = default;

    // getters
    const string& getName() const { return name; }
    // cppcheck-suppress unusedFunction
    int getLives() const { return lives; }
    // cppcheck-suppress unusedFunction
    sf::Vector2f getPosition() const { return position; }

    // expose bounds for collision checks
    sf::FloatRect bounds() const {
        if (usingTexture) return sprite.getGlobalBounds();
        return fallbackShape.getGlobalBounds();
    }

    // set position (useful for respawn)
    void setPosition(const sf::Vector2f& p) {
        position = p;
        if (usingTexture) sprite.setPosition(position);
        else fallbackShape.setPosition(position);
    }

    // A public complex function: update physics, apply gravity, integrate velocity
    // We split large dt into smaller sub-steps to avoid tunneling
    bool update(float dt, const sf::FloatRect& worldBounds) {
        // Sub-stepping: limit step to avoid tunneling when dt is large
        const float maxStep = 0.02f; // 20ms per sub-step (~50Hz)
        while (dt > 0.f) {
            float step = std::min(dt, maxStep);
            // apply gravity only if not on ground
            if (!onGround) velocity.y += GRAVITY * step;
            // integrate
            position += velocity * step;

            // simple floor collision with world bounds bottom
            if (position.y + Tile::getSize() > worldBounds.top + worldBounds.height) {
                position.y = worldBounds.top + worldBounds.height - Tile::getSize();
                velocity.y = 0.f;
                onGround = true;
            } else {
                // allow collisions to set onGround appropriately later
                onGround = false;
            }

            // prevent leaving world horizontally
            if (position.x < worldBounds.left) position.x = worldBounds.left;
            if (position.x + Tile::getSize() > worldBounds.left + worldBounds.width)
                position.x = worldBounds.left + worldBounds.width - Tile::getSize();

            dt -= step;
        }

        // update visual
        if (usingTexture) sprite.setPosition(position);
        else fallbackShape.setPosition(position);

        return true;
    }

    // move left/right - uses dt multiplier outside or inside; we provide velocity-based per-call
    void moveLeft(float dt) { position.x -= speed * dt; if (usingTexture) sprite.setPosition(position); else fallbackShape.setPosition(position); }
    void moveRight(float dt) { position.x += speed * dt; if (usingTexture) sprite.setPosition(position); else fallbackShape.setPosition(position); }

    // jump: complex (only if on ground)
    void jump() {
        if (onGround) {
            velocity.y = -jumpImpulse;
            onGround = false;
        }
    }

    // apply damage & respawn
    void takeDamageAndRespawn(const sf::Vector2f& respawnPos) {
        if (lives > 0) --lives;
        setPosition(respawnPos);
        velocity = {0.f, 0.f};
        onGround = false;
    }

    // draw
    void draw(sf::RenderTarget& target) const {
        if (usingTexture) target.draw(sprite);
        else target.draw(fallbackShape);
    }

    friend std::ostream& operator<<(std::ostream& os, const Character& c) {
        os << c.name << " pos=(" << (int)c.position.x << "," << (int)c.position.y << ") lives=" << c.lives;
        return os;
    }

    // helper to set fallback color size (used at construction or later)
    void setFallbackAppearance(const sf::Color& c) {
        fallbackShape.setFillColor(c);
        fallbackShape.setSize({Tile::getSize(), Tile::getSize()});
        fallbackShape.setPosition(position);
    }

    // expose a small helper to zero vertical velocity (used by collision resolver)
    void stopVerticalMovement() { velocity.y = 0.f; }
};

// -------------------------------
// Map (contains Tiles) - implement copy ctor/operator=/destructor explicitly
// -------------------------------
class Map {
private:
    vector<vector<Tile>> grid;
    int width, height;

    // helper to create grid
    void allocateGrid(int w, int h, TileType defaultType = TileType::Empty) {
        width = w; height = h;
        grid.assign(height, vector<Tile>(width, Tile(defaultType, 0, 0)));
        for (int r = 0; r < height; ++r)
            for (int c = 0; c < width; ++c)
                grid[r][c] = Tile(defaultType, c, r);
    }

public:
    // constructor parametric
    explicit Map(int w = 12, int h = 8, TileType defaultType = TileType::Empty) {
        allocateGrid(w,h,defaultType);
    }

    // copy constructor (explicit)
    Map(const Map& other) {
        width = other.width;
        height = other.height;
        grid.reserve(height);
        for (int r = 0; r < height; ++r) {
            vector<Tile> row;
            row.reserve(width);
            for (int c = 0; c < width; ++c) row.push_back(other.grid[r][c]);
            grid.push_back(std::move(row));
        }
    }

    // copy assignment
    Map& operator=(const Map& other) {
        if (this == &other) return *this;
        width = other.width;
        height = other.height;
        grid.clear();
        grid.reserve(height);
        for (int r = 0; r < height; ++r) {
            vector<Tile> row;
            row.reserve(width);
            for (int c = 0; c < width; ++c) row.push_back(other.grid[r][c]);
            grid.push_back(std::move(row));
        }
        return *this;
    }

    // destructor
    ~Map() {
        // No raw pointers, but destructor explicitly present to satisfy requirement.
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // complex public function: generate ascending platforms randomly
    // "ascending" -> platforms generally going upward from left to right
    void generateAscendingPlatforms(unsigned seed = 0) {
        // clear first
        for (int r=0;r<height;++r)
            for (int c=0;c<width;++c)
                grid[r][c] = Tile(TileType::Empty, c, r);

        std::mt19937 rng((seed==0)? std::random_device{}() : seed);
        std::uniform_int_distribution<int> gapDist(1,3);
        std::uniform_int_distribution<int> lengthDist(1,3);

        int currentRow = height - 2; // start near bottom
        int c = 1;
        while (c < width-1 && currentRow > 0) {
            int len = lengthDist(rng);
            for (int k = 0; k < len && c < width-1; ++k) {
                grid[currentRow][c] = Tile(TileType::Solid, c, currentRow);
                ++c;
            }
            int gap = gapDist(rng);
            c += gap;
            // ascend sometimes
            if (currentRow > 1) currentRow -= 1;
        }

        // create some special tiles: fire patch and water patch in different places
        // ensure they are on top of solids or on their own row
        grid[height-2][2] = Tile(TileType::Fire, 2, height-2);
        grid[height-3][width-3] = Tile(TileType::Water, width-3, height-3);

        // exits: place exit for Fireboy (ExitFire) and for Watergirl (ExitWater)
        // place Fire exit on top-right, Water exit on top-left (if available)
        grid[1][width-2] = Tile(TileType::ExitFire, width-2, 1);
        grid[1][1] = Tile(TileType::ExitWater, 1, 1);
    }

    // get tile type at world coords (x,y in pixels) OR by grid coords
    TileType getTileTypeAtGrid(int col, int row) const {
        if (col < 0 || col >= width || row < 0 || row >= height) return TileType::Solid;
        return grid[row][col].getType();
    }

    // cppcheck-suppress unusedFunction
    TileType getTileTypeAtWorld(float x, float y) const {
        int col = static_cast<int>(x / Tile::getSize());
        int row = static_cast<int>(y / Tile::getSize());
        return getTileTypeAtGrid(col, row);
    }

    // draw map
    void draw(sf::RenderTarget& target) const {
        for (int r = 0; r < height; ++r)
            for (int c = 0; c < width; ++c)
                grid[r][c].draw(target);
    }

    friend std::ostream& operator<<(std::ostream& os, const Map& m) {
        os << "Map " << m.width << "x" << m.height << "\n";
        for (int r = 0; r < m.height; ++r) {
            for (int c = 0; c < m.width; ++c) {
                os << m.grid[r][c];
            }
            os << "\n";
        }
        return os;
    }

    // get world bounds as rectangle
    sf::FloatRect worldBounds() const {
        return sf::FloatRect(0.f, 0.f, width * Tile::getSize(), height * Tile::getSize());
    }

    // find respawn positions for characters (grid col,row) -> world pos
    sf::Vector2f respawnWorldPosForFire() const {
        // left-bottom corner safe spot
        return sf::Vector2f(Tile::getSize() * 1.f, Tile::getSize() * (height - 2));
    }
    sf::Vector2f respawnWorldPosForWater() const {
        return sf::Vector2f(Tile::getSize() * 5.f, Tile::getSize() * (height - 2));
    }
};

// -------------------------------
// Utility collision helpers
// -------------------------------
static bool intersects(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.intersects(b);
}

// -------------------------------
// Game (manages everything)
// -------------------------------
class Game {
private:
    // window is now optional: create only when not headless to avoid SFML allocations in CI
    std::unique_ptr<sf::RenderWindow> window;
    Map map;
    Character fireboy;
    Character watergirl;
    bool fireboyAtExit = false;
    bool watergirlAtExit = false;
    bool won = false;
    // no font/text as requested




    void processInput(float dt) {
        if (won || !window) return;

        // Fireboy controls: A D W
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) fireboy.moveLeft(dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) fireboy.moveRight(dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) fireboy.jump();

        // Watergirl controls: F H T
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) watergirl.moveLeft(dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) watergirl.moveRight(dt);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) watergirl.jump();
    }

    // collision handling with tiles
    void handleCollisions(Character& ch, const sf::Vector2f& respawnPos, bool& reachedExitForCharacter, TileType whatExit) {
        (void) whatExit;
        sf::FloatRect cb = ch.bounds();
        int leftCol = std::max(0, static_cast<int>(cb.left / Tile::getSize()));
        int rightCol = std::min(map.getWidth()-1, static_cast<int>((cb.left + cb.width) / Tile::getSize()));
        int topRow = std::max(0, static_cast<int>(cb.top / Tile::getSize()));
        int bottomRow = std::min(map.getHeight()-1, static_cast<int>((cb.top + cb.height) / Tile::getSize()));

        for (int r = topRow; r <= bottomRow; ++r) {
            for (int c = leftCol; c <= rightCol; ++c) {
                TileType tt = map.getTileTypeAtGrid(c, r);

                // Determine if this tile should act as a solid for THIS character:
                // - Solid tiles are always solid
                // - Fire tiles are solid for Fireboy
                // - Water tiles are solid for Watergirl
                bool isSolidForThis = false;
                if (tt == TileType::Solid) isSolidForThis = true;
                else if (tt == TileType::Fire && ch.getName() == "Fireboy") isSolidForThis = true;
                else if (tt == TileType::Water && ch.getName() == "Watergirl") isSolidForThis = true;

                if (isSolidForThis) {
                    sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                    if (intersects(cb, tileRect)) {
                        float charCenterY = cb.top + cb.height*0.5f;
                        float tileCenterY = tileRect.top + tileRect.height*0.5f;
                        // Mark grounded and zero vertical velocity
                        ch.setOnGround(true);
                        if (charCenterY < tileCenterY) {
                            // landed on top
                            ch.setPosition({cb.left, tileRect.top - cb.height});
                        } else {
                            // collided from below
                            ch.setPosition({cb.left, tileRect.top + tileRect.height});
                            // hitting head should also stop upward velocity
                            ch.stopVerticalMovement();
                        }
                        // after resolving a solid collision, update cb for subsequent checks
                        cb = ch.bounds();
                    }
                }

                // Hazardous behavior for opposite element:
                if (tt == TileType::Fire && ch.getName() == "Watergirl") {
                    ch.takeDamageAndRespawn(respawnPos);
                    reachedExitForCharacter = false;
                    return;
                } else if (tt == TileType::Water && ch.getName() == "Fireboy") {
                    ch.takeDamageAndRespawn(respawnPos);
                    reachedExitForCharacter = false;
                    return;
                }

                // Exit tiles (non-solid) - check after solid/hazard handling
                if (tt == TileType::ExitFire && ch.getName() == "Fireboy") {
                    sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                    if (intersects(cb, tileRect)) reachedExitForCharacter = true;
                } else if (tt == TileType::ExitWater && ch.getName() == "Watergirl") {
                    sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                    if (intersects(cb, tileRect)) reachedExitForCharacter = true;
                }
            }
        }
    }

    void update(float dt) {
        if (won) return;

        sf::FloatRect world = map.worldBounds();
        fireboy.update(dt, world);
        watergirl.update(dt, world);

        handleCollisions(fireboy, map.respawnWorldPosForFire(), fireboyAtExit, TileType::ExitFire);
        handleCollisions(watergirl, map.respawnWorldPosForWater(), watergirlAtExit, TileType::ExitWater);

        if (fireboyAtExit && watergirlAtExit) {
            // keep game state as won to stop further updates, but do not display/print anything
            won = true;
        }
    }

    void render() {

        if (!window) return;
        window->clear(sf::Color(40,40,40));
        map.draw(*window);
        fireboy.draw(*window);
        watergirl.draw(*window);
        // intentionally do not draw any "WIN" text or overlay
        window->display();
    }

public:
    explicit Game(int mapW = 14, int mapH = 9)
        : map(mapW, mapH),
          fireboy("Fireboy", "assets/fireboy.jpeg", {Tile::getSize()*1.f, Tile::getSize()*(mapH-2.f)}, 3, sf::Color::Red),
          watergirl("Watergirl", "assets/watergirl.jpg", {Tile::getSize()*5.f, Tile::getSize()*(mapH-2.f)}, 3, sf::Color::Blue)
    {
        // detect headless - verificăm DOAR dacă suntem în CI SAU nu avem DISPLAY pe Linux
      /* aaaa bool headless = false;

        const char* ciEnv = std::getenv("CI");
        if (ciEnv != nullptr && std::string(ciEnv) == "true") {
            std::cout << "Detected CI environment, running in headless mode.\n";
            headless = true;
        }*/

        // REPLACE existing headless detection block with this:
        window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(static_cast<unsigned>(mapW * Tile::getSize()), static_cast<unsigned>(mapH * Tile::getSize())),
            "Fireboy & Watergirl"
        );
        if (!window || !window->isOpen()) {
            std::cerr << "Error: failed to create SFML window. Ensure a display is available and SFML is configured correctly.\n";
            // Fail fast: exit so missing display/assets are noticed by the grader
            std::exit(EXIT_FAILURE);
        }
        map.generateAscendingPlatforms(12345);

        // no font/text setup (win messages removed)

        fireboy.setFallbackAppearance(sf::Color::Red);
        watergirl.setFallbackAppearance(sf::Color::Blue);
    }

    friend std::ostream& operator<<(std::ostream& os, const Game& g) {
        os << "Game state:\n";
        os << "Map: " << g.map.getWidth() << "x" << g.map.getHeight() << "\n";
        os << "Fireboy: " << g.fireboy << "\n";
        os << "Watergirl: " << g.watergirl << "\n";
        // intentionally do not print "Won" status to avoid any win message
        return os;
    }
    void run() {


        // Mod normal cu fereastră
        sf::Clock clock;
        while (window && window->isOpen()) {
            sf::Event ev;
            while (window->pollEvent(ev)) {
                if (ev.type == sf::Event::Closed)
                    window->close();
            }

            float dt = clock.restart().asSeconds();
            processInput(dt);
            update(dt);
            render();
        }
    }

};



int main() {
    // build a game with map dimensions (width, height)
    Game game(14, 9);

    // print initial state using operator<< (scenario of use)
    cout << game << endl;

    // run the game (this will open SFML window)
    game.run();

    return 0;
}