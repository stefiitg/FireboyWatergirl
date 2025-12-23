#include "Game.h"
#include <iostream>

namespace {
    inline bool intersects(const sf::FloatRect& a, const sf::FloatRect& b) {
        return a.intersects(b);
    }
}

void Game::processInput(float dt) {
    if (won || !window) return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) fireboy->moveLeft(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) fireboy->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) fireboy->jump();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) watergirl->moveLeft(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) watergirl->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) watergirl->jump();
}

void Game::handleCollisions(Character& ch, const sf::Vector2f& respawnPos,
                            bool& reachedExitForCharacter, TileType whatExit) {
    (void) whatExit;
    sf::FloatRect cb = ch.bounds();
    int leftCol = std::max(0, static_cast<int>(cb.left / Tile::getSize()));
    int rightCol = std::min(map.getWidth()-1, static_cast<int>((cb.left + cb.width) / Tile::getSize()));
    int topRow = std::max(0, static_cast<int>(cb.top / Tile::getSize()));
    int bottomRow = std::min(map.getHeight()-1, static_cast<int>(cb.top + cb.height) / Tile::getSize());

    for (int r = topRow; r <= bottomRow; ++r) {
        for (int c = leftCol; c <= rightCol; ++c) {
            TileType tt = map.getTileTypeAtGrid(c, r);

            bool isSolidForThis = false;
            if (tt == TileType::Solid) isSolidForThis = true;
            else if (tt == TileType::Fire && ch.element() == Element::Fire) isSolidForThis = true;
            else if (tt == TileType::Water && ch.element() == Element::Water) isSolidForThis = true;

            if (isSolidForThis) {
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

            if (tt == TileType::Fire && ch.element() == Element::Water) {
                ch.takeDamageAndRespawn(respawnPos);
                reachedExitForCharacter = false;
                return;
            } else if (tt == TileType::Water && ch.element() == Element::Fire) {
                ch.takeDamageAndRespawn(respawnPos);
                reachedExitForCharacter = false;
                return;
            }

            if (tt == TileType::ExitFire && ch.element() == Element::Fire) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                if (intersects(cb, tileRect)) reachedExitForCharacter = true;
            } else if (tt == TileType::ExitWater && ch.element() == Element::Water) {
                sf::FloatRect tileRect(c * Tile::getSize(), r * Tile::getSize(), Tile::getSize(), Tile::getSize());
                if (intersects(cb, tileRect)) reachedExitForCharacter = true;
            }
        }
    }
}

void Game::update(float dt) {
    if (won) return;
    sf::FloatRect world = map.worldBounds();
    fireboy->update(dt, world);
    watergirl->update(dt, world);

    handleCollisions(*fireboy, map.respawnWorldPosForFire(), fireboyAtExit, TileType::ExitFire);
    handleCollisions(*watergirl, map.respawnWorldPosForWater(), watergirlAtExit, TileType::ExitWater);

    if (fireboyAtExit && watergirlAtExit) {
        won = true;
    }
}

void Game::render() {
    if (!window) return;
    window->clear(sf::Color(40,40,40));
    map.draw(*window);
    fireboy->draw(*window);
    watergirl->draw(*window);
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
    fireboy->setFallbackAppearance(sf::Color::Red);
    watergirl->setFallbackAppearance(sf::Color::Blue);
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
