
#include "Tile.h"

std::string toString(TileType t) {
    switch (t) {
        case TileType::Empty: return "Empty";
        case TileType::Solid: return "Solid";
        case TileType::Fire: return "Fire";
        case TileType::Water: return "Water";
        case TileType::HalfFire: return "HalfFire";
        case TileType::HalfWater: return "HalfWater";
        case TileType::Coin: return "Coin";
        case TileType::FireCoin: return "FireCoin";
        case TileType::WaterCoin: return "WaterCoin";
        case TileType::ExitFire: return "ExitFire";
        case TileType::ExitWater: return "ExitWater";
        case TileType::ExitEarth: return "ExitEarth";
    }
    return "Unknown";
}

Tile::Tile(TileType t, int col, int row)
    : type_(t), col_(col), row_(row)
{
    shape_.setSize(sf::Vector2f(static_cast<float>(getSize()), static_cast<float>(getSize())));
    shape_.setPosition(static_cast<float>(col * getSize()), static_cast<float>(row * getSize()));
    //change
    // Set colors based on tile type
    switch (type_) {
        case TileType::Empty:
            shape_.setFillColor(sf::Color::Transparent);
            break;
        case TileType::Solid:
            shape_.setFillColor(sf::Color(100, 100, 100));
            break;
        case TileType::Fire:
            shape_.setFillColor(sf::Color::Red);
            break;
        case TileType::Water:
            shape_.setFillColor(sf::Color::Blue);
            break;
        case TileType::HalfFire:
            // culoare orientativă; randarea reală desenează două jumătăți
            shape_.setFillColor(sf::Color::Red);
            break;
        case TileType::HalfWater:
            // culoare orientativă; randarea reală desenează două jumătăți
            shape_.setFillColor(sf::Color::Blue);
            break;
        case TileType::Coin:
            // culoare orientativă; randarea reală desenează doar mijlocul jumătății superioare
            shape_.setFillColor(sf::Color(200, 200, 0));
            break;
        case TileType::FireCoin:
            // orientativ
            shape_.setFillColor(sf::Color(255, 200, 120));
            break;
        case TileType::WaterCoin:
            // orientativ
            shape_.setFillColor(sf::Color::Cyan);
            break;
        case TileType::ExitFire:
            shape_.setFillColor(sf::Color(255, 165, 0)); // Orange
            break;
        case TileType::ExitWater:
            shape_.setFillColor(sf::Color::Cyan);
            break;
        case TileType::ExitEarth:
            shape_.setFillColor(sf::Color::Green);
            break;
    }
}

void Tile::draw(sf::RenderTarget& target) const {
    // Nu desenăm nimic pentru plăcile Empty
    if (type_ == TileType::Empty) return;

    // Pentru Fire/Water și HalfFire/HalfWater, desenăm două jumătăți:
    //  - jumătatea superioară: culoarea actuală (roșu/albastru)
    //  - jumătatea inferioară: gri (ca la Solid)
    if (type_ == TileType::Fire || type_ == TileType::Water ||
        type_ == TileType::HalfFire || type_ == TileType::HalfWater) {
        const sf::Vector2f pos = shape_.getPosition();
        const sf::Vector2f size = shape_.getSize();

        const float halfH = size.y / 2.f;

        sf::RectangleShape top(sf::Vector2f(size.x, halfH));
        top.setPosition(pos);
        bool isFireTop = (type_ == TileType::Fire || type_ == TileType::HalfFire);
        top.setFillColor(isFireTop ? sf::Color::Red : sf::Color::Blue);

        sf::RectangleShape bottom(sf::Vector2f(size.x, halfH));
        bottom.setPosition(sf::Vector2f(pos.x, pos.y + halfH));
        bottom.setFillColor(sf::Color(100, 100, 100));

        // Ordinea de desen nu contează mult aici, dar desenăm întâi partea de jos
        target.draw(bottom);
        target.draw(top);
        return;
    }

    // Coin-uri: desenăm doar zona de monedă (mijlocul jumătății superioare)
    if (type_ == TileType::Coin || type_ == TileType::FireCoin || type_ == TileType::WaterCoin) {
        const sf::Vector2f pos = shape_.getPosition();
        const sf::Vector2f size = shape_.getSize();
        const float halfH = size.y / 2.f; // jumătatea superioară
        const float quarterW = size.x / 4.f;
        const float coinLeft = pos.x + quarterW;      // exclude 1/4 stânga
        const float coinWidth = size.x / 2.f;         // mijloc (1/2 lățime)
        const float coinTop = pos.y;                  // în jumătatea superioară
        const float coinHeight = halfH;               // toată jumătatea superioară

        sf::RectangleShape coin(sf::Vector2f(coinWidth, coinHeight));
        coin.setPosition(sf::Vector2f(coinLeft, coinTop));
        // culoare in functie de tip
        if (type_ == TileType::FireCoin) {
            // portocaliu deschis
            coin.setFillColor(sf::Color(255, 200, 120));
        } else if (type_ == TileType::WaterCoin) {
            coin.setFillColor(sf::Color::Cyan);
        } else {
            // Coin generic (compatibilitate): auriu
            coin.setFillColor(sf::Color(255, 215, 0));
        }
        // opțional o margine ușoară
        coin.setOutlineThickness(1.f);
        if (type_ == TileType::WaterCoin) {
            coin.setOutlineColor(sf::Color(0, 120, 160));
        } else if (type_ == TileType::FireCoin) {
            coin.setOutlineColor(sf::Color(200, 120, 60));
        } else {
            coin.setOutlineColor(sf::Color(160, 120, 0));
        }
        target.draw(coin);
        return;
    }

    // Pentru celelalte tipuri, desenăm forma standard
    target.draw(shape_);
}

std::ostream& operator<<(std::ostream& os, const Tile& t) {
    os << toString(t.type_);
    return os;
}