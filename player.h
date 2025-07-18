#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "constants.h"

class Map; // Forward declaration

class Player {
public:
    sf::Texture texture;
    sf::Sprite sprite = sf::Sprite(texture);
    sf::RectangleShape fallbackRect;  // Fallback rectangle for when texture fails
    sf::Vector2f velocity;
    float speed = 200.0f;
    float maxSpeed = 200.0f;
    float sprintMultiplier = 3.0f;  // 3x speed when sprinting
    float acceleration = 800.0f;
    float friction = 600.0f;
    bool movingLeft = false;
    bool movingRight = false;
    bool movingUp = false;
    bool movingDown = false;
    bool sprinting = false;  // Sprint state
    bool useSimpleGraphics = false;  // Fallback flag

    Player();

    void findSafeSpawnPosition();
    void update(float dt, const Map& gameMap);
    void setMovement(bool left, bool right, bool up, bool down);
    void setSprinting(bool isSprinting);

    sf::Vector2f getPosition() const;
    sf::Vector2f getWorldPosition() const;
    void setPosition(const sf::Vector2f& position);

private:
    float getCurrentMaxSpeed() const;
};

#endif