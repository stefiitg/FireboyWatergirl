#ifndef OOP_BUTTON_H
#define OOP_BUTTON_H

#include <SFML/Graphics.hpp>
#include <string>

enum class ButtonState { Idle, Hover, Pressed };

class Button {
private:
    ButtonState state{ButtonState::Idle};
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color idleColor{};
    sf::Color hoverColor{};
    sf::Color activeColor{};

public:
    Button(float x, float y, float width, float height,
           sf::Font& font, const std::string& text,
           unsigned int characterSize,
           sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor);

    void update(const sf::Vector2f& mousePos);
    void render(sf::RenderTarget& target) const;
    bool isPressed() const;
};

#endif // OOP_BUTTON_H
