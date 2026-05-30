#include "Button.h"

Button::Button(float x, float y, float width, float height,
               sf::Font& font, const std::string& txt,
               unsigned int characterSize,
               sf::Color idleCol, sf::Color hoverCol, sf::Color activeCol)
    : state(ButtonState::Idle), idleColor(idleCol), hoverColor(hoverCol), activeColor(activeCol)
{
    // Shape setup
    shape.setPosition(x, y);
    shape.setSize({width, height});
    shape.setFillColor(idleColor);

    // Text setup
    text.setFont(font);
    text.setString(txt);
    text.setCharacterSize(characterSize);
    text.setFillColor(sf::Color::White);

    // Center text inside the shape
    sf::FloatRect textBounds = text.getLocalBounds();
    // Adjust origin to the visual center of the text
    text.setOrigin(textBounds.left + textBounds.width / 2.f,
                   textBounds.top + textBounds.height / 2.f);
    sf::Vector2f shapePos = shape.getPosition();
    sf::Vector2f shapeSize = shape.getSize();
    text.setPosition(shapePos.x + shapeSize.x / 2.f, shapePos.y + shapeSize.y / 2.f);
}

void Button::update(const sf::Vector2f& mousePos) {
    state = ButtonState::Idle;
    shape.setFillColor(idleColor);

    if (shape.getGlobalBounds().contains(mousePos)) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            state = ButtonState::Pressed;
            shape.setFillColor(activeColor);
        } else {
            state = ButtonState::Hover;
            shape.setFillColor(hoverColor);
        }
    }
}

void Button::render(sf::RenderTarget& target) const {
    target.draw(shape);
    target.draw(text);
}

bool Button::isPressed() const {
    return state == ButtonState::Pressed;
}
