#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include "constants.h"
#include "map.h"
#include "player.h"
#include "ui.h"

int main() {
    // 2560x1440 fullscreen
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Biome Explorer - Realistic Terrain Generation", sf::State::Fullscreen);
    window.setFramerateLimit(60);

    sf::View camera(sf::Vector2f(1280, 720), sf::Vector2f(2560, 1440));

    Map gameMap;
    Player player;
    UI ui;

    sf::Clock clock;

    std::cout << "Optimized Biome Explorer loaded!" << std::endl;
    std::cout << "Performance optimizations:" << std::endl;
    std::cout << "- Smaller world size" << std::endl;
    std::cout << "- Reduced chunk loading" << std::endl;
    std::cout << "- Simple graphics fallback" << std::endl;
    std::cout << "- Optimized collision detection" << std::endl;
    std::cout << "- Cached noise generation" << std::endl;
    std::cout << "- Natural mountain generation" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- WASD to move" << std::endl;
    std::cout << "- SHIFT to sprint (3x speed)" << std::endl;
    std::cout << "- M for map" << std::endl;
    std::cout << "- ESC to quit" << std::endl;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (event->is<sf::Event::KeyPressed>()) {
                sf::Keyboard::Key key = event->getIf<sf::Event::KeyPressed>()->code;

                if (key == sf::Keyboard::Key::M) {
                    ui.toggleMap();
                }
                else if (key == sf::Keyboard::Key::Escape) {
                    if (ui.isMapOpen()) {
                        ui.closeMap();
                    }
                    else {
                        window.close();
                    }
                }
            }
        }

        // Input (only if map is not open)
        if (!ui.isMapOpen()) {
            bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
            bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
            bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
            bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
            bool sprint = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

            player.setMovement(left, right, up, down);
            player.setSprinting(sprint);

            // Update
            player.update(dt, gameMap);

            // Chunk management (limited per frame)
            gameMap.loadChunksAroundPlayer(player.getPosition());
            gameMap.unloadDistantChunks(player.getPosition());
        }
        else {
            // Stop movement when map is open
            player.setMovement(false, false, false, false);
            player.setSprinting(false);
        }

        ui.update(player, gameMap.loadedChunks.size());

        // Camera
        sf::Vector2f playerPos = player.getPosition();
        camera.setCenter(playerPos);
        window.setView(camera);

        // Draw
        window.clear(sf::Color::Black);
        gameMap.draw(window, camera);

        // Draw player (either sprite or fallback rectangle)
        if (player.useSimpleGraphics) {
            window.draw(player.fallbackRect);
        }
        else {
            window.draw(player.sprite);
        }

        ui.draw(window, player, gameMap);

        window.display();
    }

    return 0;
}