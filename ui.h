#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "constants.h"
#include "utils.h"
#include "player.h"
#include "map.h"

class UI {
public:
    sf::Font font;
    sf::Text positionText;
    sf::Text chunkText;
    sf::Text instructionText;
    sf::Text fpsText;
    sf::Clock fpsTimer;
    int frameCount = 0;
    float fpsUpdateInterval = 1.0f;

    // Map system
    std::unordered_map<ChunkCoord, bool, ChunkCoordHash> exploredChunks;
    bool mapOpen = false;
    sf::RectangleShape minimapBackground;
    sf::RectangleShape mapBackground;
    sf::View mapView;

    // Minimap settings
    static const int MINIMAP_SIZE = 200;
    static const int MINIMAP_TILE_SIZE = 2;
    static const int MINIMAP_RANGE = 25;

    // Full map settings
    static const int MAP_TILE_SIZE = 8;

    UI();

    void markChunkExplored(ChunkCoord chunk);
    sf::Color getBiomeColor(BiomeType biome);
    void drawMinimap(sf::RenderWindow& window, const Player& player, const Map& gameMap);
    void drawFullMap(sf::RenderWindow& window, const Player& player, const Map& gameMap);

    void toggleMap();
    void closeMap();
    bool isMapOpen() const;

    void update(const Player& player, int loadedChunks);
    void draw(sf::RenderWindow& window, const Player& player, const Map& gameMap);
};

#endif