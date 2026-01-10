#include "Game.h"
#include <iostream>
#include <utility>

namespace {
    inline bool intersects(const sf::FloatRect& a, const sf::FloatRect& b) {
        return a.intersects(b);
    }
}

// Constructor de copiere: copiere profunda pentru pointerii polimorfici si reconstruirea ferestrei
Game::Game(const Game& other)
    : map(other.map),
      fireboyAtExit(other.fireboyAtExit),
      watergirlAtExit(other.watergirlAtExit),
      won(other.won),
      gameOver(other.gameOver),
      totalCoins(other.totalCoins),
      collectedCoins(other.collectedCoins),
      winFont(other.winFont),
      winText(other.winText),
      winFontLoaded(other.winFontLoaded),
      loseText(other.loseText)
{
    // Reconstruim fereastra pe baza dimensiunilor jocului/copiei
    unsigned int wPx = static_cast<unsigned>(map.getWidth() * Tile::getSize());
    unsigned int hPx = static_cast<unsigned>(map.getHeight() * Tile::getSize());
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(wPx, hPx), "Fireboy & Watergirl");

    // Cloneaza personajele si prototipurile
    if (other.fireboy) fireboy = other.fireboy->clone();
    if (other.watergirl) watergirl = other.watergirl->clone();
    if (other.fireboyPrototype) fireboyPrototype = other.fireboyPrototype->clone();
    if (other.watergirlPrototype) watergirlPrototype = other.watergirlPrototype->clone();

    // Asigura corect fontul in textele copiate (sf::Text tine un pointer catre font)
    if (winFontLoaded) {
        winText.setFont(winFont);
        loseText.setFont(winFont);
    }
}

// Operator de atribuire prin copy-and-swap
Game& Game::operator=(Game other) {
    this->swap(other);
    return *this;
}

void Game::processInput(float dt) {
    if (!window) return;

    // Permite resetarea nivelului cu tasta R cand s-a castigat sau pierdut
    if (won || gameOver) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            resetLevel();
        }
        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) fireboy->moveLeft(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) fireboy->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) fireboy->jump();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) watergirl->moveLeft(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) watergirl->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) watergirl->jump();
}

void Game::handleCollisions(Character& ch, const sf::Vector2f& respawnPos,
                            bool& reachedExitForCharacter, TileType whatExit) {
    // Parametri nefolositi in noua logica (respawn-ul nu mai este folosit cand e game over)
    (void) respawnPos;
    (void) whatExit;
    sf::FloatRect cb = ch.bounds();
    int leftCol = std::max(0, static_cast<int>(cb.left / Tile::getSize()));
    int rightCol = std::min(map.getWidth()-1, static_cast<int>((cb.left + cb.width) / Tile::getSize()));
    int topRow = std::max(0, static_cast<int>(cb.top / Tile::getSize()));
    int bottomRow = std::min(map.getHeight()-1, static_cast<int>(cb.top + cb.height) / Tile::getSize());

    for (int r = topRow; r <= bottomRow; ++r) {
        for (int c = leftCol; c <= rightCol; ++c) {
            TileType tt = map.getTileTypeAtGrid(c, r);

            // Tratare solidă pe baza polimorfismului
            if (ch.isSolidOn(tt)) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                if (intersects(cb, tileRect)) {
                    float charCenterY = cb.top + cb.height*0.5f;
                    float tileCenterY = tileRect.top + tileRect.height*0.5f;
                    ch.setOnGround(true);
                    if (charCenterY < tileCenterY) {
                        ch.setPosition({cb.left, tileRect.top - cb.height});
                    } else {
                        ch.setPosition({cb.left, tileRect.top + tileRect.height});
                        ch.stopVerticalMovement();
                    }
                    cb = ch.bounds();
                }
            }

            // Tratare specială pentru HalfFire / HalfWater (jumătate inferioară solidă, jumătate superioară poate fi letală)
            if (tt == TileType::HalfFire || tt == TileType::HalfWater) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                const float halfH = Tile::getSize() * 0.5f;
                sf::FloatRect topRect(tileRect.left, tileRect.top, tileRect.width, halfH);
                sf::FloatRect bottomRect(tileRect.left, tileRect.top + halfH, tileRect.width, halfH);

                // Partea de jos: solidă pentru toți
                if (intersects(cb, bottomRect)) {
                    float charCenterY = cb.top + cb.height * 0.5f;
                    float tileCenterY = bottomRect.top + bottomRect.height * 0.5f;
                    ch.setOnGround(true);
                    if (charCenterY < tileCenterY) {
                        ch.setPosition({cb.left, bottomRect.top - cb.height});
                    } else {
                        ch.setPosition({cb.left, bottomRect.top + bottomRect.height});
                        ch.stopVerticalMovement();
                    }
                    cb = ch.bounds();
                }

                // Partea de sus: periculoasă în funcție de personaj (polimorfic)
                if (intersects(cb, topRect)) {
                    if (ch.isTopHalfDeadly(tt)) {
                        gameOver = true;
                        reachedExitForCharacter = false;
                        return;
                    }
                }
            }

            // Monedă: zona activă este mijlocul jumătății superioare.
            // Nu este solidă; la atingere, moneda dispare și se contorizează.
            if (tt == TileType::Coin) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                const float halfH = Tile::getSize() * 0.5f;
                const float quarterW = Tile::getSize() * 0.25f;
                sf::FloatRect coinRect(tileRect.left + quarterW, tileRect.top,
                                       Tile::getSize() * 0.5f, halfH);
                if (intersects(cb, coinRect)) {
                    // colecteaza moneda o singura data
                    collectedCoins++;
                    map.setTileTypeAtGrid(c, r, TileType::Empty);
                    // Recalculeaza bounds-ul daca pozitia s-a schimbat (nu se schimba aici)
                }
            }

            if (ch.isDeadlyOn(tt)) {
                // A atins un tile letal pentru acest personaj -> joc pierdut
                gameOver = true;
                reachedExitForCharacter = false;
                return;
            }

            if (ch.canExitThrough(tt)) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                if (intersects(cb, tileRect)) reachedExitForCharacter = true;
            }
        }
    }
}

void Game::update(float dt) {
    if (won || gameOver) return;
    sf::FloatRect world = map.worldBounds();
    fireboy->update(dt, world);
    watergirl->update(dt, world);

    handleCollisions(*fireboy, map.respawnWorldPosForFire(), fireboyAtExit, TileType::ExitFire);
    handleCollisions(*watergirl, map.respawnWorldPosForWater(), watergirlAtExit, TileType::ExitWater);

    if (fireboyAtExit && watergirlAtExit && collectedCoins >= totalCoins) {
        won = true;
    }
}

void Game::render() {
    if (!window) return;
    window->clear(sf::Color(40,40,40));
    map.draw(*window);
    fireboy->draw(*window);
    watergirl->draw(*window);
    // Daca s-a castigat jocul, afiseaza mesajul "WIN"
    if (won) {
        // overlay semi-transparent
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
            // Fallback: seteaza titlul ferestrei ca "WIN"
            window->setTitle("WIN");
        }
    }
    // Daca jocul este pierdut, afiseaza mesajul "TRY AGAIN!"
    if (gameOver) {
        // overlay semi-transparent
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
      map(mapW, mapH),
      fireboy(std::make_unique<FireboyCharacter>(
          "Fireboy", "assets/fireboy.jpeg",
          sf::Vector2f{static_cast<float>(Tile::getSize()) * 1.f,
                       static_cast<float>(Tile::getSize()) * (mapH - 2.f)},
          3, sf::Color::Red)),
      watergirl(std::make_unique<WatergirlCharacter>(
          "Watergirl", "assets/watergirl.jpg",
          sf::Vector2f{static_cast<float>(Tile::getSize()) * 5.f,
                       static_cast<float>(Tile::getSize()) * (mapH - 2.f)},
          3, sf::Color::Blue))
{
    if (!window->isOpen()) {
        std::cerr << "Error: failed to create SFML window. Ensure a display is available and SFML is configured correctly.\n";
        std::exit(EXIT_FAILURE);
    }

    map.generateAscendingPlatforms(12345);
    // calculeaza totalul de monede initiale pe harta
    totalCoins = 0;
    collectedCoins = 0;
    for (int rr = 0; rr < map.getHeight(); ++rr) {
        for (int cc = 0; cc < map.getWidth(); ++cc) {
            if (map.getTileTypeAtGrid(cc, rr) == TileType::Coin) totalCoins++;
        }
    }
    fireboy->setFallbackAppearance(sf::Color::Red);
    watergirl->setFallbackAppearance(sf::Color::Blue);

    // Salveaza prototipurile initiale (pentru resetare prin clone)
    fireboyPrototype = fireboy->clone();
    watergirlPrototype = watergirl->clone();

    // Incarca font pentru mesajul de castig
    // Incercam fonturi uzuale din Windows
    const char* candidates[] = {
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\verdana.ttf"
    };
    for (const char* path : candidates) {
        if (winFont.loadFromFile(path)) { winFontLoaded = true; break; }
    }

    if (winFontLoaded) {
        winText.setFont(winFont);
        winText.setString("WIN");
        // Dimensiunea textului proportional cu inaltimea ferestrei
        unsigned int size = static_cast<unsigned int>(std::max(30.f, (window->getSize().y * 0.2f)));
        winText.setCharacterSize(size);
        winText.setFillColor(sf::Color::Yellow);
        winText.setOutlineThickness(4.f);
        winText.setOutlineColor(sf::Color::Black);
        // Pozitia exacta va fi recalculata in render() pentru a ramane centrata
        // Configureaza si textul pentru ecranul de pierdere
        loseText.setFont(winFont);
        loseText.setString("TRY AGAIN!");
        loseText.setCharacterSize(size);
        loseText.setFillColor(sf::Color(220, 20, 60)); // crimson/rosu
        loseText.setOutlineThickness(4.f);
        loseText.setOutlineColor(sf::Color::Black);
    } else {
        std::cerr << "Warning: Could not load a system font for WIN text. Title fallback will be used.\n";
    }
}

void Game::resetLevel() {
    // Regenerare harta si resetare monede
    map.generateAscendingPlatforms(12345);
    totalCoins = 0;
    collectedCoins = 0;
    for (int rr = 0; rr < map.getHeight(); ++rr) {
        for (int cc = 0; cc < map.getWidth(); ++cc) {
            if (map.getTileTypeAtGrid(cc, rr) == TileType::Coin) totalCoins++;
        }
    }

    // Recreeaza personajele din prototipuri
    if (fireboyPrototype) fireboy = fireboyPrototype->clone();
    if (watergirlPrototype) watergirl = watergirlPrototype->clone();

    // Reseteaza pozitiile la punctele de respawn
    fireboy->setPosition(map.respawnWorldPosForFire());
    watergirl->setPosition(map.respawnWorldPosForWater());

    // Reaplica fallback appearance (in cazul build-urilor fara texturi)
    fireboy->setFallbackAppearance(sf::Color::Red);
    watergirl->setFallbackAppearance(sf::Color::Blue);

    // Reset flags
    won = false;
    gameOver = false;
    fireboyAtExit = false;
    watergirlAtExit = false;
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << "Game state:\n";
    os << "Map: " << g.map.getWidth() << "x" << g.map.getHeight() << "\n";
    os << "Fireboy: " << *g.fireboy << "\n";
    os << "Watergirl: " << *g.watergirl << "\n";
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
        processInput(dt);
        update(dt);
        render();
    }
}
