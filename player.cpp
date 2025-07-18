#include "player.h"
#include "map.h"
#include <cmath>
#include <iostream>
#include <algorithm>

Player::Player() {
    // Try to load player texture
    if (!texture.loadFromFile("textures/player.png")) {
        std::cout << "Could not load player texture, using simple rectangle..." << std::endl;
        useSimpleGraphics = true;

        // Create a simple colored rectangle as fallback
        fallbackRect.setSize(sf::Vector2f(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f));
        fallbackRect.setFillColor(sf::Color::Red);
        fallbackRect.setOrigin({ TILE_SIZE * 0.4f, TILE_SIZE * 0.4f }); // Center the sprite
    }
    else {
        // Set up sprite with loaded texture
        sprite.setTexture(texture);
        sprite = sf::Sprite(texture);

        // Scale the sprite to fit tile size (assuming player texture is smaller)
        sf::Vector2u textureSize = texture.getSize();
        if (textureSize.x > 0 && textureSize.y > 0) {
            float scaleX = (TILE_SIZE * 0.8f) / textureSize.x;
            float scaleY = (TILE_SIZE * 0.8f) / textureSize.y;
            sprite.setScale({ scaleX, scaleY });
        }

        // Center the sprite
        /*sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin({ bounds.size.x / 2.0f, bounds.size.y / 2.0f });*/
    }

    // Find a safe spawn position in plains
    findSafeSpawnPosition();
}

void Player::findSafeSpawnPosition() {
    // Start from center and spiral outward to find plains
    int centerX = WORLD_WIDTH / 2;
    int centerY = WORLD_HEIGHT / 2;

    for (int radius = 0; radius < 100; radius++) {
        for (int angle = 0; angle < 360; angle += 10) {
            int x = centerX + static_cast<int>(radius * std::cos(angle * M_PI / 180));
            int y = centerY + static_cast<int>(radius * std::sin(angle * M_PI / 180));

            if (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT) {
                // Check if this position is in plains (grassland)
                Map tempMap; // Temporary map for biome checking
                BiomeType biome = tempMap.determineBiome(x, y);
                TileType tileType = tempMap.generateTileType(x, y);

                if (biome == BiomeType::GRASSLAND && tileType == TileType::GRASS) {
                    setPosition({ static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE) });
                    return;
                }
            }
        }
    }

    // Fallback to center if no plains found
    setPosition({ static_cast<float>(WORLD_WIDTH * TILE_SIZE / 2), static_cast<float>(WORLD_HEIGHT * TILE_SIZE / 2) });
}

float Player::getCurrentMaxSpeed() const {
    return sprinting ? maxSpeed * sprintMultiplier : maxSpeed;
}

void Player::setPosition(const sf::Vector2f& position) {
    if (useSimpleGraphics) {
        fallbackRect.setPosition(position);
    }
    else {
        sprite.setPosition(position);
    }
}

sf::Vector2f Player::getPosition() const {
    if (useSimpleGraphics) {
        return fallbackRect.getPosition();
    }
    else {
        return sprite.getPosition();
    }
}

void Player::update(float dt, const Map& gameMap) {
    float currentMaxSpeed = getCurrentMaxSpeed();

    // Calculate target velocity
    sf::Vector2f targetVelocity(0.0f, 0.0f);

    if (movingLeft) targetVelocity.x -= currentMaxSpeed;
    if (movingRight) targetVelocity.x += currentMaxSpeed;
    if (movingUp) targetVelocity.y -= currentMaxSpeed;
    if (movingDown) targetVelocity.y += currentMaxSpeed;

    // Normalize diagonal movement
    if (targetVelocity.x != 0 && targetVelocity.y != 0) {
        float length = std::sqrt(targetVelocity.x * targetVelocity.x + targetVelocity.y * targetVelocity.y);
        targetVelocity.x = (targetVelocity.x / length) * currentMaxSpeed;
        targetVelocity.y = (targetVelocity.y / length) * currentMaxSpeed;
    }

    // Smooth velocity interpolation with adjusted acceleration for sprinting
    sf::Vector2f velocityDiff = targetVelocity - velocity;
    float changeAmount = (targetVelocity.x == 0 && targetVelocity.y == 0) ? friction : acceleration;

    // Increase acceleration when sprinting for more responsive feel
    if (sprinting && (targetVelocity.x != 0 || targetVelocity.y != 0)) {
        changeAmount *= 1.5f;
    }

    velocity.x += (velocityDiff.x > 0 ? 1 : -1) * std::min(std::abs(velocityDiff.x), changeAmount * dt);
    velocity.y += (velocityDiff.y > 0 ? 1 : -1) * std::min(std::abs(velocityDiff.y), changeAmount * dt);

    // Movement with collision detection
    sf::Vector2f originalPos = getPosition();

    // Try horizontal movement
    sf::Vector2f newPos = originalPos + sf::Vector2f(velocity.x * dt, 0);
    setPosition(newPos);

    // Simple collision check (just check player center)
    int tileX = static_cast<int>(newPos.x / TILE_SIZE);
    int tileY = static_cast<int>(newPos.y / TILE_SIZE);

    if (gameMap.isTileSolid(tileX, tileY)) {
        setPosition({ originalPos.x, newPos.y });
        velocity.x = 0;
    }

    // Try vertical movement
    sf::Vector2f currentPos = getPosition();
    newPos = currentPos + sf::Vector2f(0, velocity.y * dt);
    setPosition(newPos);

    tileX = static_cast<int>(newPos.x / TILE_SIZE);
    tileY = static_cast<int>(newPos.y / TILE_SIZE);

    if (gameMap.isTileSolid(tileX, tileY)) {
        setPosition({ currentPos.x, originalPos.y });
        velocity.y = 0;
    }

    // World bounds
    sf::Vector2f pos = getPosition();
    if (pos.x < 0) setPosition({ 0, pos.y });
    if (pos.y < 0) setPosition({ pos.x, 0 });
    if (pos.x > WORLD_WIDTH * TILE_SIZE) setPosition({ static_cast<float>(WORLD_WIDTH * TILE_SIZE), pos.y });
    if (pos.y > WORLD_HEIGHT * TILE_SIZE) setPosition({ pos.x, static_cast<float>(WORLD_HEIGHT * TILE_SIZE) });
}

void Player::setMovement(bool left, bool right, bool up, bool down) {
    movingLeft = left;
    movingRight = right;
    movingUp = up;
    movingDown = down;
}

void Player::setSprinting(bool isSprinting) {
    sprinting = isSprinting;
}

sf::Vector2f Player::getWorldPosition() const {
    sf::Vector2f pos = getPosition();
    return sf::Vector2f(pos.x / TILE_SIZE, pos.y / TILE_SIZE);
}