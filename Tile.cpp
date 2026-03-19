
#include "Tile.h"


static sf::Texture g_halfWaterTex;
static bool g_halfWaterTexTried = false;
static bool g_halfWaterTexOk = false;


static sf::Texture g_halfFireTex;
static bool g_halfFireTexTried = false;
static bool g_halfFireTexOk = false;


sf::Texture Tile::solidTex;
bool Tile::solidTexLoaded = false;
bool Tile::solidTexOk = false;

sf::Texture Tile::exitFireTex;
sf::Texture Tile::exitWaterTex;
sf::Texture Tile::exitEarthTex;
sf::Texture Tile::exitAirTex;
bool Tile::exitTexturesLoaded = false;
bool Tile::exitFireLoaded = false;
bool Tile::exitWaterLoaded = false;
bool Tile::exitEarthLoaded = false;
bool Tile::exitAirLoaded = false;

void Tile::ensureExitTexturesLoaded() {
    if (exitTexturesLoaded) return;
    // incercam sa incarcam fiecare textura independent; pastam flags
    // nu flosim throw daca incarcarea esueaza, fallback la dreptunghiuriile initiale in caz contrar
    exitFireLoaded = exitFireTex.loadFromFile("assets/exit_fireboy.png");
    exitWaterLoaded = exitWaterTex.loadFromFile("assets/exit_watergirl.png");
    exitEarthLoaded = exitEarthTex.loadFromFile("assets/exit_earthboy.png");
    exitAirLoaded = exitAirTex.loadFromFile("assets/exit_airgirl.png");
    exitTexturesLoaded = true;
}

void Tile::ensureSolidTextureLoaded() {
    if (solidTexLoaded) return;
    solidTexLoaded = true;
    solidTexOk = solidTex.loadFromFile("assets/solid.png");
}

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
        case TileType::EarthCoin: return "EarthCoin";
        case TileType::ExitFire: return "ExitFire";
        case TileType::ExitWater: return "ExitWater";
        case TileType::ExitEarth: return "ExitEarth";
        case TileType::ExitAir: return "ExitAir";
    }
    return "Unknown";
}

Tile::Tile(TileType t, int col, int row)
    : type_(t), col_(col), row_(row)
{
    shape_.setSize(sf::Vector2f(static_cast<float>(getSize()), static_cast<float>(getSize())));
    shape_.setPosition(static_cast<float>(col * getSize()), static_cast<float>(row * getSize()));

    // setam culorile in functie de tilee
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
            // culoare orientativa
            shape_.setFillColor(sf::Color::Red);
            break;
        case TileType::HalfWater:

            shape_.setFillColor(sf::Color::Blue);
            break;
        case TileType::Coin:
            // culoare orientativa, randarea reala deseneaza doar mijlocul jumatații superioare
            shape_.setFillColor(sf::Color(200, 200, 0));
            break;
        case TileType::FireCoin:
            // orientativ
            shape_.setFillColor(sf::Color(255, 200, 120));
            break;
        case TileType::WaterCoin:

            shape_.setFillColor(sf::Color::Cyan);
            break;
        case TileType::EarthCoin:

            shape_.setFillColor(sf::Color::Green);
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
        case TileType::ExitAir:
            shape_.setFillColor(sf::Color::White);
            break;
    }
}

void Tile::draw(sf::RenderTarget& target) const {
    // nu se deseneaza nimic pt placile empty
    if (type_ == TileType::Empty) return;

    // randare textured pt solid tiles cu fallback
    if (type_ == TileType::Solid) {
        ensureSolidTextureLoaded();
        if (solidTexOk) {
            sf::Sprite s;
            s.setTexture(solidTex);
            s.setPosition(shape_.getPosition());
            const auto texSize = solidTex.getSize();
            if (texSize.x > 0 && texSize.y > 0) {
                const float scaleX = Tile::getSize() / static_cast<float>(texSize.x);
                const float scaleY = Tile::getSize() / static_cast<float>(texSize.y);
                s.setScale(scaleX, scaleY);
            }
            target.draw(s);
            return; // desenare textured solid
        }
        // daca textura nu se incarca- ia culoarea gri de la inceput
    }

    // tratament special pt Halfwater : render cu textura daca exista
    if (type_ == TileType::HalfWater) {
        if (!g_halfWaterTexTried) {
            g_halfWaterTexTried = true;
            g_halfWaterTexOk = g_halfWaterTex.loadFromFile("assets/half_water.png");
        }

        if (g_halfWaterTexOk) {
            sf::Sprite s;
            s.setTexture(g_halfWaterTex);
            s.setPosition(shape_.getPosition());
            const auto texSize = g_halfWaterTex.getSize();
            if (texSize.x > 0 && texSize.y > 0) {
                const float scaleX = Tile::getSize() / static_cast<float>(texSize.x);
                const float scaleY = Tile::getSize() / static_cast<float>(texSize.y);
                s.setScale(scaleX, scaleY);
            }
            target.draw(s);
            return; // desenare halfwaterr
        }
        // daca textura nu se incarca, fallback
    }

    // tratare speciala pt half fire
    if (type_ == TileType::HalfFire) {
        if (!g_halfFireTexTried) {
            g_halfFireTexTried = true;
            g_halfFireTexOk = g_halfFireTex.loadFromFile("assets/half_fire.png");
        }

        if (g_halfFireTexOk) {
            sf::Sprite s;
            s.setTexture(g_halfFireTex);
            s.setPosition(shape_.getPosition());
            const auto texSize = g_halfFireTex.getSize();
            if (texSize.x > 0 && texSize.y > 0) {
                const float scaleX = Tile::getSize() / static_cast<float>(texSize.x);
                const float scaleY = Tile::getSize() / static_cast<float>(texSize.y);
                s.setScale(scaleX, scaleY);
            }
            target.draw(s);
            return; // -textura lui halffire(si halfwater) se deseneaza ca un tile intreg
        }
        //daca textura esuaza , fallback la jumate rosu jumate gri
    }

    // textura pt tile urile exit
    if (type_ == TileType::ExitFire || type_ == TileType::ExitWater || type_ == TileType::ExitEarth || type_ == TileType::ExitAir) {
        ensureExitTexturesLoaded();

        const sf::Texture* tex = nullptr;
        bool ok = false;
        switch (type_) {
            case TileType::ExitFire:  tex = &exitFireTex;  ok = exitFireLoaded;  break;
            case TileType::ExitWater: tex = &exitWaterTex; ok = exitWaterLoaded; break;
            case TileType::ExitEarth: tex = &exitEarthTex; ok = exitEarthLoaded; break;
            case TileType::ExitAir:   tex = &exitAirTex;   ok = exitAirLoaded;   break;
            default: break;
        }

        if (ok && tex && tex->getSize().y > 0) {
            sf::Sprite sprite;
            sprite.setTexture(*tex);
            float factor = Tile::getSize() / static_cast<float>(tex->getSize().y);
            sprite.setScale(factor, factor);
            sprite.setPosition(shape_.getPosition());
            target.draw(sprite);
            return;
        }
        // daca textura lipseste, fallback la dreptunghiul gri
    }

    // Pentru Fire/Water și HalfFire/HalfWater, desenăm două jumătăți:
    //  - jumatatea superioara: culoarea actuală (roșu/albastru)
    //  - jumatatea inferioara: gri (ca la Solid)
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

    // coin-uri: desenam doar zona de monedă (mijlocul jumatații superioare)
    if (type_ == TileType::Coin || type_ == TileType::FireCoin || type_ == TileType::WaterCoin || type_ == TileType::EarthCoin) {
        const sf::Vector2f pos = shape_.getPosition();
        const sf::Vector2f size = shape_.getSize();
        const float halfH = size.y / 2.f; // jumatatea superioara
        const float quarterW = size.x / 4.f;
        const float coinLeft = pos.x + quarterW;      // exclude 1/4 stânga
        const float coinWidth = size.x / 2.f;         // mijloc (1/2 lățime)
        const float coinTop = pos.y;                  // an jumatatea superioara
        const float coinHeight = halfH;               // toată jumatatea superioara

        sf::RectangleShape coin(sf::Vector2f(coinWidth, coinHeight));
        coin.setPosition(sf::Vector2f(coinLeft, coinTop));
        // culoare in functie de tip
        if (type_ == TileType::FireCoin) {
            // portocaliu deschis pt fireboy
            coin.setFillColor(sf::Color(255, 200, 120));
        } else if (type_ == TileType::WaterCoin) {
            coin.setFillColor(sf::Color::Cyan);
        } else if (type_ == TileType::EarthCoin) {
            coin.setFillColor(sf::Color::Green);
        } else {
            // coin generic (compatibilitate): auriu
            coin.setFillColor(sf::Color(255, 215, 0));
        }
        // o margine ușoară- pt dimensiune
        coin.setOutlineThickness(1.f);
        if (type_ == TileType::WaterCoin) {
            coin.setOutlineColor(sf::Color(0, 120, 160));
        } else if (type_ == TileType::FireCoin) {
            coin.setOutlineColor(sf::Color(200, 120, 60));
        } else if (type_ == TileType::EarthCoin) {
            coin.setOutlineColor(sf::Color(0, 100, 0));
        } else {
            coin.setOutlineColor(sf::Color(160, 120, 0));
        }
        target.draw(coin);
        return;
    }

    // ptr celelalte tipuri desenam forma standard
    target.draw(shape_);
}

std::ostream& operator<<(std::ostream& os, const Tile& t) {
    os << toString(t.type_);
    return os;
}