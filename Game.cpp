#include "Game.h"
#include "GameExceptions.h"
#include "MathUtils.h"
#include "ResourceManager.h"
#include "CharacterFactory.h"
#include <iostream>
#include <utility>
#include <algorithm>

namespace {
    inline bool intersects(const sf::FloatRect& a, const sf::FloatRect& b) {
        return a.intersects(b);
    }

    inline void resolveCollision(Character& ch, sf::FloatRect& cb, const sf::FloatRect& rect) {
        float charCenterX = cb.left + cb.width * 0.5f;
        float rectCenterX = rect.left + rect.width * 0.5f;
        float charCenterY = cb.top + cb.height * 0.5f;
        float rectCenterY = rect.top + rect.height * 0.5f;

        float dx = charCenterX - rectCenterX;
        float dy = charCenterY - rectCenterY;

        float intersectX = (cb.width + rect.width) * 0.5f - std::abs(dx);
        float intersectY = (cb.height + rect.height) * 0.5f - std::abs(dy);

        if (intersectX > 0.0f && intersectY > 0.0f) {
            if (intersectX < intersectY) {
                // Lateral/Horizontal resolution
                if (dx > 0.0f) {
                    ch.setPosition({rect.left + rect.width, cb.top});
                } else {
                    ch.setPosition({rect.left - cb.width, cb.top});
                }
            } else {
                // Vertical resolution
                if (dy > 0.0f) {
                    ch.setPosition({cb.left, rect.top + rect.height});
                    ch.stopVerticalMovement();
                } else {
                    ch.setPosition({cb.left, rect.top - cb.height});
                    ch.setOnGround(true);
                }
            }
            cb = ch.bounds();
        }
    }
}

void Game::initializeCharacters() {
    // clear all containers
    characters.clear();
    characterPrototypes.clear();
    charactersAtExit.clear();
    spawnPositions.clear();
    characterControls.clear();

    // Demonstrate template class instantiation for textures
    ResourceManager<sf::Texture>::getInstance().getResource("assets/fireboy1.png");

    // 1) Fireboy
    {
        sf::Vector2f spawn = map.respawnWorldPosForFire();
        auto fb = CharacterFactory::createCharacter(PlayerType::Fireboy, spawn);
        if (!fb->isUsingTexture()) {
            throw ResourceLoadError("Failed to load mandatory asset: assets/fireboy1.png");
        }
        fb->setFallbackAppearance();
        spawnPositions.push_back(spawn);
        characters.push_back(std::move(fb));
        charactersAtExit.push_back(false);
        characterControls.push_back(Controls{sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W});
    }

    // 2) Watergirl
    {
        sf::Vector2f spawn = map.respawnWorldPosForWater();
        auto wg = CharacterFactory::createCharacter(PlayerType::Watergirl, spawn);
        if (!wg->isUsingTexture()) {
            throw ResourceLoadError("Failed to load mandatory asset: assets/watergirl1.png");
        }
        wg->setFallbackAppearance();
        spawnPositions.push_back(spawn);
        characters.push_back(std::move(wg));
        charactersAtExit.push_back(false);
        characterControls.push_back(Controls{sf::Keyboard::F, sf::Keyboard::H, sf::Keyboard::T});
    }

    // 3) Earthboy
    {
        sf::Vector2f spawn = map.respawnWorldPosForEarth();
        auto eb = CharacterFactory::createCharacter(PlayerType::Earthboy, spawn);
        if (!eb->isUsingTexture()) {
            throw ResourceLoadError("Failed to load mandatory asset: assets/earthboy.png");
        }
        eb->setFallbackAppearance();
        spawnPositions.push_back(spawn);
        characters.push_back(std::move(eb));
        charactersAtExit.push_back(false);
        characterControls.push_back(Controls{sf::Keyboard::J, sf::Keyboard::L, sf::Keyboard::I});
    }

    // 4) Airgirl
    {
        sf::Vector2f spawn = map.respawnWorldPosForAir();
        auto ag = CharacterFactory::createCharacter(PlayerType::Airgirl, spawn);
        if (!ag->isUsingTexture()) {
            throw ResourceLoadError("Failed to load mandatory asset: assets/airgirl.png");
        }
        ag->setFallbackAppearance();
        spawnPositions.push_back(spawn);
        characters.push_back(std::move(ag));
        charactersAtExit.push_back(false);
        characterControls.push_back(Controls{sf::Keyboard::Z, sf::Keyboard::C, sf::Keyboard::X});
    }

    // fill prototypes by cloning active characters
    for (const auto& ch : characters) {
        if (ch) characterPrototypes.push_back(ch->clone());
        else characterPrototypes.push_back(nullptr);
    }
}


Game::Game(const Game& other)
    : map(other.map),
      won(other.won),
      gameOver(other.gameOver),
      totalCoins(other.totalCoins),
      collectedCoins(other.collectedCoins),
      winFont(other.winFont),
      winText(other.winText),
      winFontLoaded(other.winFontLoaded),
      loseText(other.loseText)
{
    unsigned int wPx = static_cast<unsigned>(map.getWidth() * Tile::getSize());
    unsigned int hPx = static_cast<unsigned>(map.getHeight() * Tile::getSize());
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(wPx, hPx), "Fireboy & Watergirl");

    // deep copy vectors
    characters.clear();
    characterPrototypes.clear();
    charactersAtExit = other.charactersAtExit;
    spawnPositions = other.spawnPositions;
    characterControls = other.characterControls;

    for (const auto& ch : other.characters) {
        if (ch) characters.push_back(ch->clone());
        else characters.push_back(nullptr);
    }
    for (const auto& proto : other.characterPrototypes) {
        if (proto) characterPrototypes.push_back(proto->clone());
        else characterPrototypes.push_back(nullptr);
    }

    if (winFontLoaded) {
        winText.setFont(winFont);
        loseText.setFont(winFont);
    }
}

//operator de atribuire prin copy and swap
Game& Game::operator=(Game other) {
    this->swap(other);
    return *this;
}

void Game::processInput(float dt) {
    if (!window) return;


    if (won || gameOver) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            resetLevel();
        }
        return;
    }

    // generic controls processing for all characters
    const size_t n = std::min(characterControls.size(), characters.size());
    for (size_t i = 0; i < n; ++i) {
        if (!characters[i]) continue;
        const Controls& ctl = characterControls[i];
        if (sf::Keyboard::isKeyPressed(ctl.left)) characters[i]->moveLeft(dt);
        if (sf::Keyboard::isKeyPressed(ctl.right)) characters[i]->moveRight(dt);
        if (sf::Keyboard::isKeyPressed(ctl.jump)) characters[i]->jump();
    }
}

bool Game::handleCollisions(Character& ch) {
    bool reachedExitForCharacter = false;
    sf::FloatRect cb = ch.bounds();
    int maxCol = map.getWidth() - 1;
    int maxRow = map.getHeight() - 1;
    int leftCol = clamp<int>(static_cast<int>(cb.left / Tile::getSize()), 0, maxCol);
    int rightCol = clamp<int>(static_cast<int>((cb.left + cb.width) / Tile::getSize()), 0, maxCol);
    int topRow = clamp<int>(static_cast<int>(cb.top / Tile::getSize()), 0, maxRow);
    int bottomRow = clamp<int>(static_cast<int>((cb.top + cb.height) / Tile::getSize()), 0, maxRow);

    for (int r = topRow; r <= bottomRow; ++r) {
        for (int c = leftCol; c <= rightCol; ++c) {
            TileType tt = map.getTileTypeAtGrid(c, r);

            // tratare solida pe baza polimorfismului
            if (ch.isSolidOn(tt)) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                if (intersects(cb, tileRect)) {
                    resolveCollision(ch, cb, tileRect);
                }
            }

//tratare speciala pt half fire si half water
            if (tt == TileType::HalfFire || tt == TileType::HalfWater) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                const float halfH = Tile::getSize() * 0.5f;
                sf::FloatRect topRect(tileRect.left, tileRect.top, tileRect.width, halfH);
                sf::FloatRect bottomRect(tileRect.left, tileRect.top + halfH, tileRect.width, halfH);

                // partea de jos e solida pt toti
                if (intersects(cb, bottomRect)) {
                    resolveCollision(ch, cb, bottomRect);
                }


                if (intersects(cb, topRect)) { //partea de sus este letala/ok
                    if (ch.isTopHalfDeadly(tt)) {
                        gameOver = true;

                        return false;
                    }
                }
            }

            // moneda-zona activa e mijlocul jumatatii superioare
            if (tt == TileType::Coin || tt == TileType::FireCoin || tt == TileType::WaterCoin || tt == TileType::EarthCoin) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                const float halfH = Tile::getSize() * 0.5f;
                const float quarterW = Tile::getSize() * 0.25f;
                sf::FloatRect coinRect(tileRect.left + quarterW, tileRect.top,
                                       Tile::getSize() * 0.5f, halfH);
                if (intersects(cb, coinRect)) {
                    // colectare conditionata de tipul jucatorului- downcast
                    bool canCollect = false;
                    if (tt == TileType::Coin) {

                        canCollect = true;
                    } else if (tt == TileType::FireCoin) {
                        const auto* asFire = dynamic_cast<const FireboyCharacter*>(&ch);
                        canCollect = (asFire != nullptr);
                    } else if (tt == TileType::WaterCoin) {
                        const auto* asWater = dynamic_cast<const WatergirlCharacter*>(&ch);
                        canCollect = (asWater != nullptr);
                    } else if (tt == TileType::EarthCoin) {
                        const auto* asEarth = dynamic_cast<const EarthboyCharacter*>(&ch);
                        canCollect = (asEarth != nullptr);
                    }
                    if (canCollect) {
                        collectedCoins++;
                        map.setTileTypeAtGrid(c, r, TileType::Empty);
                    }

                }
            }

            if (ch.isDeadlyOn(tt)) {
   //a atins un tile letal=> game over
                gameOver = true;

                return false;
            }

            if (ch.canExitThrough(tt)) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                if (intersects(cb, tileRect)) reachedExitForCharacter = true;
            }
        }
    }

    //  also handle miscarea platformei
    handlePlatformCollisions(ch);
    return reachedExitForCharacter;
}

void Game::update(float dt) {
    if (won || gameOver) { // still keep HUD updated
        gameHud.update(currentLevel, collectedCoins, totalCoins);
        return;
    }

    map.update(dt);
    sf::FloatRect world = map.worldBounds();

    // reset exit flags
    charactersAtExit.assign(characters.size(), false);

    // update movement/physics
    for (auto& ch : characters) {
        if (ch) ch->update(dt, world);
    }

    // collisions and exits/coins
    for (size_t i = 0; i < characters.size(); ++i) {
        if (!characters[i]) continue;
        charactersAtExit[i] = handleCollisions(*characters[i]);
    }

    // win condition: all at exit and coins collected
    if (!charactersAtExit.empty()) {
        bool allAtExit = std::all_of(charactersAtExit.begin(), charactersAtExit.end(), [](bool v){ return v; });
        if (allAtExit && collectedCoins >= totalCoins) {
            won = true;
        }
    }

    // update HUD at the end
    gameHud.update(currentLevel, collectedCoins, totalCoins);
}

void Game::handlePlatformCollisions(Character& ch) {

    sf::FloatRect cb = ch.bounds();
    for (const auto& mp : map.getMovingPlatforms()) {
        sf::FloatRect pb = mp.bounds();
        if (cb.intersects(pb)) {
            resolveCollision(ch, cb, pb);
        } else {
            // CARRY minimal
            const float epsilon = 1.5f;
            bool horizontally = (cb.left < pb.left + pb.width) && (cb.left + cb.width > pb.left);
            bool onTop = std::abs((cb.top + cb.height) - pb.top) <= epsilon;
            if (horizontally && onTop) {
                sf::Vector2f newPos = {cb.left + mp.getLastDeltaX(), cb.top};
                ch.setPosition(newPos);
                cb = ch.bounds();
            }
        }// CARRY
    }
}

void Game::render() {
    if (!window) return;
    window->clear(sf::Color(40,40,40));
    map.draw(*window);
    for (const auto& ch : characters) {
        if (ch) ch->draw(*window);
    }

    // Render HUD before overlays
    gameHud.render(*window);

    if (won) {

        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y)));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window->draw(overlay);

        if (winFontLoaded) {
            // centreaza textul in fereastra in functie de dimensiunile curente
            sf::FloatRect textRect = winText.getLocalBounds();
            winText.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
            winText.setPosition(static_cast<float>(window->getSize().x) / 2.f,
                                static_cast<float>(window->getSize().y) / 2.f);
            window->draw(winText);
        } else {

            window->setTitle("WIN");
        }
    }

    if (gameOver) {

        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y)));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window->draw(overlay);

        if (winFontLoaded) {
            sf::FloatRect textRect = loseText.getLocalBounds();
            loseText.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
            loseText.setPosition(static_cast<float>(window->getSize().x) / 2.f,
                                 static_cast<float>(window->getSize().y) / 2.f);
            window->draw(loseText);
        } else {
            window->setTitle("TRY AGAIN!");
        }
    }
    window->display();
}

Game::Game(int mapW, int mapH)
    : window(std::make_unique<sf::RenderWindow>(
          sf::VideoMode(static_cast<unsigned>(mapW * Tile::getSize()),
                        static_cast<unsigned>(mapH * Tile::getSize())),
          "Fireboy & Watergirl")),
      map(mapW, mapH)
{
    if (!window->isOpen()) {
        throw WindowCreationError("Failed to create SFML window. Ensure a display is available and SFML is configured correctly.");
    }

    // Start in Menu state; level will be loaded via startLevel() after selection
    state = GameState::Menu;

    // Load font using ResourceManager (Singleton template)
    {
        sf::Font& font = ResourceManager<sf::Font>::getInstance().getResource("assets/arial.ttf");
        // apply directly to texts
        winText.setFont(font);
        winText.setString("WIN");
        // dimensiunea textului proportional cu inaltimea ferestrei
        unsigned int size = static_cast<unsigned int>(std::max(30.f, (window->getSize().y * 0.2f)));
        winText.setCharacterSize(size);
        winText.setFillColor(sf::Color::Yellow);
        winText.setOutlineThickness(4.f);
        winText.setOutlineColor(sf::Color::Black);
        // pozitia exacta va fi recalculata in render() pentru a ramane centrata
        // configureaza si textul pentru ecranul de pierdere
        loseText.setFont(font);
        loseText.setString("TRY AGAIN!");
        loseText.setCharacterSize(size);
        loseText.setFillColor(sf::Color(220, 20, 60));
        loseText.setOutlineThickness(4.f);
        loseText.setOutlineColor(sf::Color::Black);
        // keep compatibility with existing checks
        winFontLoaded = true;

        // Initialize HUD using the loaded font
        gameHud.init(font, window->getSize().x);

        // Initialize menu buttons (four level buttons centered on screen)
        const float btnWidth = 200.f;
        const float btnHeight = 50.f;
        const float spacing = 20.f;
        const float centerX = static_cast<float>(window->getSize().x) * 0.5f;
        const float centerY = static_cast<float>(window->getSize().y) * 0.5f;
        const float totalH = 4.f * btnHeight + 3.f * spacing;
        const float startY = centerY - totalH * 0.5f; // top-aligned to center the full stack

        sf::Color idleCol(60, 60, 60);
        sf::Color hoverCol(100, 100, 100);
        sf::Color activeCol(220, 100, 50);

        auto makeButtonPosX = [&](float w){ return centerX - w * 0.5f; };

        menuButtons.clear();
        menuButtons.emplace_back(
            makeButtonPosX(btnWidth), startY,
            btnWidth, btnHeight,
            font, std::string("Level 1"), 24,
            idleCol, hoverCol, activeCol
        );
        menuButtons.emplace_back(
            makeButtonPosX(btnWidth), startY + (btnHeight + spacing),
            btnWidth, btnHeight,
            font, std::string("Level 2"), 24,
            idleCol, hoverCol, activeCol
        );
        menuButtons.emplace_back(
            makeButtonPosX(btnWidth), startY + 2.f * (btnHeight + spacing),
            btnWidth, btnHeight,
            font, std::string("Level 3"), 24,
            idleCol, hoverCol, activeCol
        );
        menuButtons.emplace_back(
            makeButtonPosX(btnWidth), startY + 3.f * (btnHeight + spacing),
            btnWidth, btnHeight,
            font, std::string("Level 4"), 24,
            idleCol, hoverCol, activeCol
        );
    }
}

void Game::resetLevel() {
    // regenerare harta si resetare
    map.loadLevel(currentLevel);
    totalCoins = 0;
    collectedCoins = 0;
    for (int rr = 0; rr < map.getHeight(); ++rr) {
        for (int cc = 0; cc < map.getWidth(); ++cc) {
            TileType t = map.getTileTypeAtGrid(cc, rr);
            if (t == TileType::Coin || t == TileType::FireCoin || t == TileType::WaterCoin || t == TileType::EarthCoin) totalCoins++;
        }
    }

    //rebuild
    characters.clear();
    for (const auto& proto : characterPrototypes) {
        if (proto) characters.push_back(proto->clone());
        else characters.push_back(nullptr);
    }
    // reset
    for (size_t i = 0; i < characters.size(); ++i) {
        if (!characters[i]) continue;
        if (i < spawnPositions.size()) characters[i]->setPosition(spawnPositions[i]);
        // fallback restructurat
        characters[i]->setFallbackAppearance();
    }

    // reset flags
    won = false;
    gameOver = false;
    charactersAtExit.assign(characters.size(), false);
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << "Game state:\n";
    os << "Map: " << g.map.getWidth() << "x" << g.map.getHeight() << "\n";
    for (size_t i = 0; i < g.characters.size(); ++i) {
        if (g.characters[i]) os << "Character[" << i << "]: " << *g.characters[i] << "\n";
    }
    return os;
}

void Game::run() {
    sf::Clock clock;
    while (window && window->isOpen()) {
        sf::Event ev;
        while (window->pollEvent(ev)) {
            if (ev.type == sf::Event::Closed)
                window->close();
        }
        float dt = clock.restart().asSeconds();
        // Clamp dt to avoid large spikes (e.g., when dragging the window) that can cause physics tunneling
        dt = clamp<float>(dt, 0.0f, 0.05f);
        if (state == GameState::Menu) {
            processMenuInput();
            renderMenu();
        } else {
            processInput(dt);
            update(dt);
            render(); //-fix eroare la dragging ul ferestrei
        }
    }
}

void Game::startLevel() {
    map.loadLevel(currentLevel);

    // recompute coins exactly as before
    totalCoins = 0;
    collectedCoins = 0;
    for (int rr = 0; rr < map.getHeight(); ++rr) {
        for (int cc = 0; cc < map.getWidth(); ++cc) {
            TileType t = map.getTileTypeAtGrid(cc, rr);
            if (t == TileType::Coin || t == TileType::FireCoin || t == TileType::WaterCoin || t == TileType::EarthCoin) totalCoins++;
        }
    }

    initializeCharacters();

    won = false;
    gameOver = false;
    charactersAtExit.assign(characters.size(), false);

    state = GameState::Playing;
}

void Game::processMenuInput() {
    if (!window) return;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
        currentLevel = LevelType::Level1;
        startLevel();
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) {
        currentLevel = LevelType::Level2;
        startLevel();
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3)) {
        currentLevel = LevelType::Level3;
        startLevel();
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) {
        currentLevel = LevelType::Level4;
        startLevel();
    }

    // Mouse-based menu interaction
    sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
    sf::Vector2f mousePos = window->mapPixelToCoords(pixelPos);

    for (auto& btn : menuButtons) {
        btn.update(mousePos);
    }

    bool leftDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if (leftDown && !isMouseHeld) {
        isMouseHeld = true;
        // Determine which button was pressed
        if (menuButtons.size() >= 4) {
            if (menuButtons[0].isPressed()) {
                currentLevel = LevelType::Level1;
                startLevel();
            } else if (menuButtons[1].isPressed()) {
                currentLevel = LevelType::Level2;
                startLevel();
            } else if (menuButtons[2].isPressed()) {
                currentLevel = LevelType::Level3;
                startLevel();
            } else if (menuButtons[3].isPressed()) {
                currentLevel = LevelType::Level4;
                startLevel();
            }
        }
    } else if (!leftDown) {
        isMouseHeld = false;
    }
}

void Game::renderMenu() {
    if (!window) return;
    window->clear(sf::Color(30, 30, 30));

    // Optional small title at the top
    if (winFontLoaded) {
        sf::Text smallTitle;
        smallTitle.setFont(ResourceManager<sf::Font>::getInstance().getResource("assets/arial.ttf"));
        smallTitle.setString("Select Level");
        unsigned int size = static_cast<unsigned int>(std::max(18.f, (window->getSize().y * 0.05f)));
        smallTitle.setCharacterSize(size);
        smallTitle.setFillColor(sf::Color::White);
        smallTitle.setOutlineThickness(2.f);
        smallTitle.setOutlineColor(sf::Color::Black);
        sf::FloatRect tr = smallTitle.getLocalBounds();
        smallTitle.setOrigin(tr.left + tr.width / 2.f, tr.top + tr.height / 2.f);
        smallTitle.setPosition(static_cast<float>(window->getSize().x) / 2.f, 40.f);
        window->draw(smallTitle);
    }

    // Render buttons
    for (const auto& btn : menuButtons) {
        btn.render(*window);
    }
    window->display();
}
