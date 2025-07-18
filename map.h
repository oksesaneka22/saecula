#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include "chunk.h"
#include "constants.h"
#include "utils.h"

class Map {
public:
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> loadedChunks;
    sf::Texture grassTexture;
    sf::Texture waterTexture;
    sf::Texture stoneTexture;
    sf::Texture treeTexture;

    // Noise cache for performance
    std::unordered_map<std::pair<int, int>, float, PairHash> noiseCache;

    // Fallback colors
    sf::RectangleShape grassTile;
    sf::RectangleShape waterTile;
    sf::RectangleShape stoneTile;
    sf::RectangleShape treeTile;
    bool useSimpleGraphics = false;

    Map();

    float noise(int x, int y, int scale);
    float getDistanceToRiver(int worldX, int worldY);
    BiomeType determineBiome(int worldX, int worldY) const;
    TileType generateTileType(int worldX, int worldY);

    // New mountain generation methods
    float getMountainHeight(int worldX, int worldY);
    bool isInMountainRange(int worldX, int worldY);
    TileType generateMountainTileType(int worldX, int worldY, float elevation, float mountainHeight);

    void loadChunk(ChunkCoord chunkCoord);
    void unloadDistantChunks(sf::Vector2f playerPos);
    void loadChunksAroundPlayer(sf::Vector2f playerPos);

    bool isTileSolid(int worldX, int worldY) const;
    void draw(sf::RenderWindow& window, sf::View& camera);
};

#endif