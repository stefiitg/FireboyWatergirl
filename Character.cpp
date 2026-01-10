#include "Character.h"
#include <algorithm>

void Character::initFallbackShape(const sf::Color& c, const sf::Vector2f& size) {
    fallbackShape.setSize(size);
    fallbackShape.setFillColor(c);
    fallbackShape.setPosition(position);
}

Character::Character(const std::string& nm, const std::string& texturePath,
                     const sf::Vector2f& pos, int lifeCount,
                     const sf::Color& fallbackColor)
    : name(nm), usingTexture(false), position(pos), velocity(0.f, 0.f),
      lives(lifeCount), onGround(false)
{
    if (!texturePath.empty() && texture.loadFromFile(texturePath)) {
        usingTexture = true;
        sprite.setTexture(texture);
        if (texture.getSize().y > 0) {
            float factor = Tile::getSize() / static_cast<float>(texture.getSize().y);
            sprite.setScale(factor, factor);
        }
        sprite.setPosition(position);
    } else {
        usingTexture = false;
        initFallbackShape(fallbackColor, {static_cast<float>(Tile::getSize()), static_cast<float>(Tile::getSize())});
    }
}

Character::~Character() = default;

void Character::setOnGround(bool state) {
    onGround = state;
    if (onGround) {
        velocity.y = 0.f;
    }
}

Character::Character(const Character& other)
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

Character& Character::operator=(const Character& other) {
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

sf::FloatRect Character::bounds() const {
    if (usingTexture) return sprite.getGlobalBounds();
    return fallbackShape.getGlobalBounds();
}

void Character::setPosition(const sf::Vector2f& p) {
    position = p;
    if (usingTexture) sprite.setPosition(position);
    else fallbackShape.setPosition(position);
}

bool Character::update(float dt, const sf::FloatRect& worldBounds) {
    const float maxStep = 0.02f;
    float remaining = dt;
    while (remaining > 0.f) {
        float step = std::min(remaining, maxStep);
        if (!onGround) velocity.y += GRAVITY * step;
        position += velocity * step;

        if (position.y + Tile::getSize() > worldBounds.top + worldBounds.height) {
            position.y = worldBounds.top + worldBounds.height - Tile::getSize();
            velocity.y = 0.f;
            onGround = true;
        } else {
            onGround = false;
        }

        if (position.x < worldBounds.left) position.x = worldBounds.left;
        if (position.x + Tile::getSize() > worldBounds.left + worldBounds.width)
            position.x = worldBounds.left + worldBounds.width - Tile::getSize();

        remaining -= step;
    }

    if (usingTexture) sprite.setPosition(position);
    else fallbackShape.setPosition(position);
    return true;
}

void Character::moveLeft(float dt) {
    position.x -= speed * dt;
    if (usingTexture) sprite.setPosition(position); else fallbackShape.setPosition(position);
}

void Character::moveRight(float dt) {
    position.x += speed * dt;
    if (usingTexture) sprite.setPosition(position); else fallbackShape.setPosition(position);
}

void Character::jump() {
    if (onGround) {
        velocity.y = -jumpImpulse;
        onGround = false;
    }
}

// takeDamageAndRespawn removed: game now transitions to Game Over instead of respawning

void Character::draw(sf::RenderTarget& target) const {
    // NVI: interfață non-virtuală, delegă la hook-ul virtual
    drawImpl(target);
}

void Character::drawImpl(sf::RenderTarget& target) const {
    if (usingTexture) target.draw(sprite);
    else target.draw(fallbackShape);
}

void Character::setFallbackAppearance(const sf::Color& c) {
    fallbackShape.setFillColor(c);
    fallbackShape.setSize({static_cast<float>(Tile::getSize()), static_cast<float>(Tile::getSize())});
    fallbackShape.setPosition(position);
}

void Character::stopVerticalMovement() { velocity.y = 0.f; }

void Character::print(std::ostream& os) const {
    os << name << " pos=(" << (int)position.x << "," << (int)position.y << ") lives=" << lives;
}

std::ostream& operator<<(std::ostream& os, const Character& c) {
    c.print(os);
    return os;
}
