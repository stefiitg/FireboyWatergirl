#ifndef OOP_MOVING_PLATFORM_H
#define OOP_MOVING_PLATFORM_H

#include <SFML/Graphics.hpp>
#include "Tile.h"

class MovingPlatform {
private:
    sf::Vector2f pos{};
    float speed = 80.f;
    int direction = 1;
    float xMin = 0.f;
    float xMax = 0.f;
    float lastDx = 0.f;
    sf::RectangleShape shape;

public:
    MovingPlatform() = default;
    MovingPlatform(const sf::Vector2f& startPos, float minX, float maxX, float spd = 80.f, int dir = 1)
        : pos(startPos), speed(spd), direction(dir), xMin(minX), xMax(maxX), lastDx(0.f) {
        shape.setSize({static_cast<float>(Tile::getSize()), static_cast<float>(Tile::getSize())});
        shape.setFillColor(sf::Color(150, 90, 30));
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(1.f);
        shape.setPosition(pos);
    }

    void update(float dt) {
        lastDx = direction * speed * dt;
        pos.x += lastDx;
        if (pos.x < xMin) {
            pos.x = xMin;
            direction *= -1;
            lastDx = 0.f; // clamped this frame
        } else if (pos.x > xMax) {
            pos.x = xMax;
            direction *= -1;
            lastDx = 0.f;
        }
        shape.setPosition(pos);
    }

    sf::FloatRect bounds() const {
        return sf::FloatRect(pos.x, pos.y, static_cast<float>(Tile::getSize()), static_cast<float>(Tile::getSize()));
    }

    void draw(sf::RenderTarget& target) const { target.draw(shape); }

    float getLastDeltaX() const { return lastDx; }
};

#endif
