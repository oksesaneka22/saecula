#include "ui.h"
#include <iostream>

UI::UI() : positionText(font), chunkText(font), instructionText(font), fpsText(font) {
    // Use default font if loading fails
    if (!font.openFromFile("fonts/arial.ttf")) {
        std::cout << "Using default font" << std::endl;
    }

    positionText.setFont(font);
    positionText.setString("Position: (0, 0)");
    positionText.setCharacterSize(16);
    positionText.setFillColor(sf::Color::White);

    chunkText.setFont(font);
    chunkText.setString("Loaded Chunks: 0");
    chunkText.setCharacterSize(16);
    chunkText.setFillColor(sf::Color::White);

    instructionText.setFont(font);
    instructionText.setString("WASD to move | SHIFT to sprint | M for map | ESC to quit | Optimized for performance!");
    instructionText.setCharacterSize(14);
    instructionText.setFillColor(sf::Color::Yellow);

    fpsText.setFont(font);
    fpsText.setString("FPS: 0");
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);

    // Setup minimap background
    minimapBackground.setSize(sf::Vector2f(MINIMAP_SIZE + 10, MINIMAP_SIZE + 10));
    minimapBackground.setFillColor(sf::Color(0, 0, 0, 150));
    minimapBackground.setOutlineColor(sf::Color::White);
    minimapBackground.setOutlineThickness(2);

    // Setup map background
    mapBackground.setSize(sf::Vector2f(1200, 800));
    mapBackground.setFillColor(sf::Color(0, 0, 0, 200));
    mapBackground.setOutlineColor(sf::Color::White);
    mapBackground.setOutlineThickness(3);
}

void UI::markChunkExplored(ChunkCoord chunk) {
    exploredChunks[chunk] = true;
}

sf::Color UI::getBiomeColor(BiomeType biome) {
    switch (biome) {
    case BiomeType::GRASSLAND: return sf::Color(34, 139, 34);
    case BiomeType::FOREST: return sf::Color(0, 100, 0);
    case BiomeType::MOUNTAIN: return sf::Color(128, 128, 128);
    case BiomeType::LAKE: return sf::Color(30, 144, 255);
    case BiomeType::RIVER: return sf::Color(30, 144, 255);
    default: return sf::Color::Black;
    }
}

void UI::drawMinimap(sf::RenderWindow& window, const Player& player, const Map& gameMap) {
    sf::Vector2f playerPos = player.getWorldPosition();

    // Position minimap in top-right corner
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f minimapPos(windowSize.x - MINIMAP_SIZE - 20, 20);

    minimapBackground.setPosition({ minimapPos.x - 5, minimapPos.y - 5 });
    window.draw(minimapBackground);

    // Draw explored chunks on minimap
    int playerChunkX = static_cast<int>(playerPos.x) / CHUNK_SIZE;
    int playerChunkY = static_cast<int>(playerPos.y) / CHUNK_SIZE;

    for (int dy = -MINIMAP_RANGE; dy <= MINIMAP_RANGE; dy++) {
        for (int dx = -MINIMAP_RANGE; dx <= MINIMAP_RANGE; dx++) {
            int chunkX = playerChunkX + dx;
            int chunkY = playerChunkY + dy;

            if (chunkX >= 0 && chunkX < CHUNKS_X && chunkY >= 0 && chunkY < CHUNKS_Y) {
                ChunkCoord chunk = { chunkX, chunkY };

                if (exploredChunks.find(chunk) != exploredChunks.end()) {
                    // Sample biome from center of chunk
                    int sampleX = chunkX * CHUNK_SIZE + CHUNK_SIZE / 2;
                    int sampleY = chunkY * CHUNK_SIZE + CHUNK_SIZE / 2;
                    BiomeType biome = gameMap.determineBiome(sampleX, sampleY);

                    sf::RectangleShape chunkRect;
                    chunkRect.setSize(sf::Vector2f(MINIMAP_TILE_SIZE, MINIMAP_TILE_SIZE));
                    chunkRect.setFillColor(getBiomeColor(biome));
                    chunkRect.setPosition(sf::Vector2f(
                        minimapPos.x + (dx + MINIMAP_RANGE) * MINIMAP_TILE_SIZE,
                        minimapPos.y + (dy + MINIMAP_RANGE) * MINIMAP_TILE_SIZE
                    ));
                    window.draw(chunkRect);
                }
            }
        }
    }

    // Draw player position on minimap with different color when sprinting
    sf::CircleShape playerDot(3);
    playerDot.setFillColor(player.sprinting ? sf::Color::Yellow : sf::Color::Red);
    playerDot.setPosition(sf::Vector2f(
        minimapPos.x + MINIMAP_RANGE * MINIMAP_TILE_SIZE - 3,
        minimapPos.y + MINIMAP_RANGE * MINIMAP_TILE_SIZE - 3
    ));
    window.draw(playerDot);
}

void UI::drawFullMap(sf::RenderWindow& window, const Player& player, const Map& gameMap) {
    if (!mapOpen) return;

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f mapPos(
        (windowSize.x - mapBackground.getSize().x) / 2,
        (windowSize.y - mapBackground.getSize().y) / 2
    );

    mapBackground.setPosition(mapPos);
    window.draw(mapBackground);

    // Draw title
    sf::Text mapTitle = sf::Text(font);
    mapTitle.setString("Explored Map (M to close)");
    mapTitle.setFont(font);
    mapTitle.setCharacterSize(20);
    mapTitle.setFillColor(sf::Color::White);
    mapTitle.setPosition({ mapPos.x + 10, mapPos.y + 10 });
    window.draw(mapTitle);

    // Draw explored chunks
    sf::Vector2f playerPos = player.getWorldPosition();
    int playerChunkX = static_cast<int>(playerPos.x) / CHUNK_SIZE;
    int playerChunkY = static_cast<int>(playerPos.y) / CHUNK_SIZE;

    // Calculate map bounds
    int mapStartX = mapPos.x + 10;
    int mapStartY = mapPos.y + 40;
    int mapWidth = mapBackground.getSize().x - 20;
    int mapHeight = mapBackground.getSize().y - 50;

    // Calculate visible chunk range
    int chunksPerRow = mapWidth / MAP_TILE_SIZE;
    int chunksPerCol = mapHeight / MAP_TILE_SIZE;

    int startChunkX = playerChunkX - chunksPerRow / 2;
    int startChunkY = playerChunkY - chunksPerCol / 2;

    for (int y = 0; y < chunksPerCol; y++) {
        for (int x = 0; x < chunksPerRow; x++) {
            int chunkX = startChunkX + x;
            int chunkY = startChunkY + y;

            if (chunkX >= 0 && chunkX < CHUNKS_X && chunkY >= 0 && chunkY < CHUNKS_Y) {
                ChunkCoord chunk = { chunkX, chunkY };

                if (exploredChunks.find(chunk) != exploredChunks.end()) {
                    // Sample biome from center of chunk
                    int sampleX = chunkX * CHUNK_SIZE + CHUNK_SIZE / 2;
                    int sampleY = chunkY * CHUNK_SIZE + CHUNK_SIZE / 2;
                    BiomeType biome = gameMap.determineBiome(sampleX, sampleY);

                    sf::RectangleShape chunkRect;
                    chunkRect.setSize(sf::Vector2f(MAP_TILE_SIZE, MAP_TILE_SIZE));
                    chunkRect.setFillColor(getBiomeColor(biome));
                    chunkRect.setPosition(sf::Vector2f(
                        mapStartX + x * MAP_TILE_SIZE,
                        mapStartY + y * MAP_TILE_SIZE
                    ));
                    window.draw(chunkRect);
                }
            }
        }
    }

    // Draw player position on full map with different color when sprinting
    sf::CircleShape playerDot(4);
    playerDot.setFillColor(player.sprinting ? sf::Color::Yellow : sf::Color::Red);
    playerDot.setOutlineColor(sf::Color::White);
    playerDot.setOutlineThickness(1);
    playerDot.setPosition(sf::Vector2f(
        mapStartX + (chunksPerRow / 2) * MAP_TILE_SIZE - 4,
        mapStartY + (chunksPerCol / 2) * MAP_TILE_SIZE - 4
    ));
    window.draw(playerDot);
}

void UI::toggleMap() {
    mapOpen = !mapOpen;
}

void UI::closeMap() {
    mapOpen = false;
}

bool UI::isMapOpen() const {
    return mapOpen;
}

void UI::update(const Player& player, int loadedChunks) {
    sf::Vector2f worldPos = player.getWorldPosition();
    std::string sprintStatus = player.sprinting ? " (SPRINTING)" : "";
    positionText.setString("Position: (" + std::to_string(static_cast<int>(worldPos.x)) +
        ", " + std::to_string(static_cast<int>(worldPos.y)) + ")" + sprintStatus);

    chunkText.setString("Loaded Chunks: " + std::to_string(loadedChunks));

    frameCount++;
    if (fpsTimer.getElapsedTime().asSeconds() >= fpsUpdateInterval) {
        float fps = frameCount / fpsTimer.getElapsedTime().asSeconds();
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        frameCount = 0;
        fpsTimer.restart();
    }

    // Mark current chunk as explored
    ChunkCoord currentChunk = {
        static_cast<int>(worldPos.x) / CHUNK_SIZE,
        static_cast<int>(worldPos.y) / CHUNK_SIZE
    };
    markChunkExplored(currentChunk);
}

void UI::draw(sf::RenderWindow& window, const Player& player, const Map& gameMap) {
    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());

    positionText.setPosition({ 10, 10 });
    chunkText.setPosition({ 10, 30 });
    instructionText.setPosition({ 10, static_cast<float>(window.getSize().y - 50) });

    sf::FloatRect fpsRect = fpsText.getLocalBounds();
    fpsText.setPosition(sf::Vector2f(
        static_cast<float>(window.getSize().x) - fpsRect.size.x - 10,
        static_cast<float>(window.getSize().y) - 30
    ));

    window.draw(positionText);
    window.draw(chunkText);
    window.draw(instructionText);
    window.draw(fpsText);

    // Draw minimap (always visible)
    drawMinimap(window, player, gameMap);

    // Draw full map (only when open)
    drawFullMap(window, player, gameMap);

    window.setView(originalView);
}