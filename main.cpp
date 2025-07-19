#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include "constants.h"
#include "map.h"
#include "player.h"
#include "ui.h"

int main() {
    // 2560x1440 fullscreen
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Biome Explorer - Crafting & Tools System", sf::State::Fullscreen);
    window.setFramerateLimit(60);

    sf::View camera({ 1280, 720 }, { 2560, 1440 });

    Map gameMap;
    Player player;
    UI ui;

    sf::Clock clock;

    std::cout << "Biome Explorer with Crafting System loaded!" << std::endl;
    std::cout << "New Features:" << std::endl;
    std::cout << "- Crafting system (C key)" << std::endl;
    std::cout << "- Tool slots for pickaxe and axe" << std::endl;
    std::cout << "- Stone harvesting with pickaxe" << std::endl;
    std::cout << "- Faster tree harvesting with axe" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- WASD to move" << std::endl;
    std::cout << "- SHIFT to sprint (3x speed)" << std::endl;
    std::cout << "- Right-click trees to harvest (within 3 tiles)" << std::endl;
    std::cout << "- Right-click stone to harvest (requires pickaxe)" << std::endl;
    std::cout << "- M for map" << std::endl;
    std::cout << "- E for inventory" << std::endl;
    std::cout << "- C for crafting" << std::endl;
    std::cout << "- Left-click in inventory to move items" << std::endl;
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
                    if (!ui.isInventoryOpen() && !ui.isCraftingOpen()) {
                        ui.toggleMap();
                    }
                }
                else if (key == sf::Keyboard::Key::E) {
                    if (!ui.isMapOpen() && !ui.isCraftingOpen()) {
                        ui.toggleInventory();
                    }
                }
                else if (key == sf::Keyboard::Key::C) {
                    if (!ui.isMapOpen() && !ui.isInventoryOpen()) {
                        ui.toggleCrafting();
                    }
                }
                else if (key == sf::Keyboard::Key::Escape) {
                    if (ui.isMapOpen()) {
                        ui.closeMap();
                    }
                    else if (ui.isInventoryOpen()) {
                        ui.closeInventory();
                    }
                    else if (ui.isCraftingOpen()) {
                        ui.closeCrafting();
                    }
                    else {
                        window.close();
                    }
                }
            }
            if (event->is<sf::Event::MouseButtonPressed>()) {
                sf::Mouse::Button button = event->getIf<sf::Event::MouseButtonPressed>()->button;
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (button == sf::Mouse::Button::Left && (ui.isInventoryOpen() || ui.isCraftingOpen())) {
                    // Handle inventory/crafting clicks - mouse pressed
                    ui.handleInventoryClick({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) }, player, true);
                }
                else if (button == sf::Mouse::Button::Right && !ui.isMapOpen() && !ui.isInventoryOpen() && !ui.isCraftingOpen()) {
                    // Get mouse position in world coordinates
                    sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos, camera);

                    // Convert to tile coordinates
                    int tileX = static_cast<int>(worldMousePos.x / TILE_SIZE);
                    int tileY = static_cast<int>(worldMousePos.y / TILE_SIZE);

                    // Check if it's a harvestable tile and within range
                    if (tileX >= 0 && tileX < WORLD_WIDTH && tileY >= 0 && tileY < WORLD_HEIGHT) {
                        TileType tileType = gameMap.generateTileType(tileX, tileY);
                        if (player.canHarvestTile(tileType) && player.isWithinHarvestRange(tileX, tileY)) {
                            if (!player.getIsHarvesting()) {
                                player.startHarvesting(tileX, tileY, {
                                    static_cast<float>(tileX * TILE_SIZE + TILE_SIZE / 2),
                                    static_cast<float>(tileY * TILE_SIZE + TILE_SIZE / 2)
                                    }, tileType);
                            }
                        }
                    }
                }
            }
            if (event->is<sf::Event::MouseButtonReleased>()) {
                sf::Mouse::Button button = event->getIf<sf::Event::MouseButtonReleased>()->button;
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (button == sf::Mouse::Button::Left && (ui.isInventoryOpen() || ui.isCraftingOpen())) {
                    // Handle inventory/crafting clicks - mouse released
                    ui.handleInventoryClick({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) }, player, false);
                }
            }
        }

        // Input (only if map, inventory, and crafting are not open)
        if (!ui.isMapOpen() && !ui.isInventoryOpen() && !ui.isCraftingOpen()) {
            bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
            bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
            bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
            bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
            bool sprint = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

            // Stop harvesting if player tries to move
            if ((left || right || up || down) && player.getIsHarvesting()) {
                player.stopHarvesting();
            }

            player.setMovement(left, right, up, down);
            player.setSprinting(sprint);

            // Update
            player.update(dt, gameMap);

            // Chunk management (limited per frame)
            gameMap.loadChunksAroundPlayer(player.getPosition());
            gameMap.unloadDistantChunks(player.getPosition());
        }
        else {
            // Stop movement when UI is open
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
