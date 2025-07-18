#include "map.h"
#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>

Map::Map() {
    // Try to load textures, fallback to simple rectangles
    if (!grassTexture.loadFromFile("textures/grass.png") ||
        !waterTexture.loadFromFile("textures/water.png") ||
        !stoneTexture.loadFromFile("textures/stone.png") ||
        !treeTexture.loadFromFile("textures/tree.png")) {

        std::cout << "Using simple graphics for better performance..." << std::endl;
        useSimpleGraphics = true;

        // Create simple colored rectangles
        grassTile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        grassTile.setFillColor(sf::Color(34, 139, 34));

        waterTile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        waterTile.setFillColor(sf::Color(30, 144, 255));

        stoneTile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        stoneTile.setFillColor(sf::Color(128, 128, 128));

        treeTile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        treeTile.setFillColor(sf::Color(0, 100, 0));
    }
}

float Map::noise(int x, int y, int scale) {
    auto key = std::make_pair(x / scale, y / scale);
    auto it = noiseCache.find(key);
    if (it != noiseCache.end()) {
        return it->second;
    }

    // Simple multi-octave noise simulation
    float result = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < 3; i++) {
        std::mt19937 rng((x / scale) * frequency * 1000 + (y / scale) * frequency + i * 10000);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        result += dist(rng) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    result /= maxValue;
    noiseCache[key] = result;
    return result;
}

float Map::getDistanceToRiver(int worldX, int worldY) {
    // Create multiple river paths
    float minDistance = 1000.0f;

    // River 1: Diagonal flow
    float riverX1 = worldX + worldY * 0.3f;
    float riverY1 = worldY - worldX * 0.2f;
    float riverNoise1 = noise(static_cast<int>(riverX1), static_cast<int>(riverY1), 50) * 30.0f;
    float distToRiver1 = std::abs(std::sin(riverX1 * 0.01f) * 50.0f + riverNoise1);

    // River 2: Horizontal meandering
    float riverNoise2 = noise(worldX, worldY, 80) * 40.0f;
    float distToRiver2 = std::abs((worldY % 300) - 150 + std::sin(worldX * 0.02f) * 30.0f + riverNoise2);

    // River 3: Vertical meandering
    float riverNoise3 = noise(worldX, worldY, 90) * 35.0f;
    float distToRiver3 = std::abs((worldX % 400) - 200 + std::sin(worldY * 0.015f) * 25.0f + riverNoise3);

    minDistance = std::min({ distToRiver1, distToRiver2, distToRiver3 });
    return minDistance;
}

float Map::getMountainHeight(int worldX, int worldY) {
    // Create multiple mountain ranges with different characteristics
    float height = 0.0f;

    // Primary mountain range - large scale ridges
    float ridge1 = std::abs(std::sin((worldX + worldY) * 0.003f)) * 0.8f;
    float ridge1Noise = noise(worldX, worldY, 200) * 0.3f;
    height = std::max(height, ridge1 + ridge1Noise);

    // Secondary mountain range - perpendicular ridges
    float ridge2 = std::abs(std::sin((worldX - worldY) * 0.004f)) * 0.7f;
    float ridge2Noise = noise(worldX + 500, worldY + 500, 150) * 0.25f;
    height = std::max(height, ridge2 + ridge2Noise);

    // Tertiary peaks - isolated mountains
    float peaks = noise(worldX, worldY, 100) * noise(worldX + 1000, worldY + 1000, 120);
    if (peaks > 0.6f) {
        height = std::max(height, peaks);
    }

    // Add fine detail noise
    float detail = noise(worldX, worldY, 50) * 0.15f;
    height += detail;

    return std::min(height, 1.0f);
}

bool Map::isInMountainRange(int worldX, int worldY) {
    float mountainHeight = getMountainHeight(worldX, worldY);
    return mountainHeight > 0.4f; // Threshold for mountain areas
}

TileType Map::generateMountainTileType(int worldX, int worldY, float elevation, float mountainHeight) {
    std::mt19937 rng(worldX * 1000 + worldY);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float random = dist(rng);

    // Higher mountain areas are more likely to be stone
    float stoneThreshold = 0.3f + (mountainHeight - 0.4f) * 1.5f; // Increases with height
    stoneThreshold = std::min(stoneThreshold, 0.9f);

    // Very high peaks are almost always stone
    if (mountainHeight > 0.8f) {
        return (random < 0.95f) ? TileType::STONE : TileType::GRASS;
    }

    // Medium height mountains have mixed stone and grass
    if (random < stoneThreshold) {
        return TileType::STONE;
    }

    // Lower mountain areas can have some trees
    if (mountainHeight < 0.6f && random < 0.1f) {
        return TileType::TREE;
    }

    return TileType::GRASS;
}

BiomeType Map::determineBiome(int worldX, int worldY) const {
    Map* mutableThis = const_cast<Map*>(this);

    float elevation = mutableThis->noise(worldX, worldY, 150);
    float moisture = mutableThis->noise(worldX + 1000, worldY + 1000, 120);
    float temperature = mutableThis->noise(worldX + 2000, worldY + 2000, 180);
    float distanceToRiver = mutableThis->getDistanceToRiver(worldX, worldY);

    // River check first
    if (distanceToRiver < 8.0f) {
        return BiomeType::RIVER;
    }

    // Lake generation in low elevation areas
    if (elevation < 0.25f && moisture > 0.4f) {
        return BiomeType::LAKE;
    }

    // Mountain generation using new mountain height system
    if (mutableThis->isInMountainRange(worldX, worldY)) {
        return BiomeType::MOUNTAIN;
    }

    // Forest generation - depends on moisture and temperature
    if (moisture > 0.55f && temperature > 0.3f && temperature < 0.8f) {
        return BiomeType::FOREST;
    }

    // Default to grassland
    return BiomeType::GRASSLAND;
}

TileType Map::generateTileType(int worldX, int worldY) {
    BiomeType biome = determineBiome(worldX, worldY);
    float elevation = noise(worldX, worldY, 150);
    float moisture = noise(worldX + 1000, worldY + 1000, 120);

    std::mt19937 rng(worldX * 1000 + worldY);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float random = dist(rng);

    switch (biome) {
    case BiomeType::LAKE:
    case BiomeType::RIVER:
        return TileType::WATER;

    case BiomeType::MOUNTAIN:
    {
        float mountainHeight = getMountainHeight(worldX, worldY);
        return generateMountainTileType(worldX, worldY, elevation, mountainHeight);
    }

    case BiomeType::FOREST:
        // Forests have varying tree density
        if (random < 0.75f) {
            return TileType::TREE;
        }
        else {
            return TileType::GRASS;
        }

    default: // GRASSLAND
        // Grasslands have sparse trees
        if (random < 0.05f) {
            return TileType::TREE;
        }
        else {
            return TileType::GRASS;
        }
    }
}

void Map::loadChunk(ChunkCoord chunkCoord) {
    if (loadedChunks.find(chunkCoord) != loadedChunks.end()) {
        return;
    }

    auto chunk = std::make_unique<Chunk>(chunkCoord);

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int worldX = chunkCoord.x * CHUNK_SIZE + x;
            int worldY = chunkCoord.y * CHUNK_SIZE + y;

            TileType tileType = generateTileType(worldX, worldY);

            if (!useSimpleGraphics) {
                sf::Texture* texture = nullptr;
                switch (tileType) {
                case TileType::WATER: texture = &waterTexture; break;
                case TileType::STONE: texture = &stoneTexture; break;
                case TileType::TREE: texture = &treeTexture; break;
                default: texture = &grassTexture; break;
                }

                chunk->tiles[y][x] = Tile(tileType, *texture);
                chunk->tiles[y][x]->sprite.setTexture(*texture);
                chunk->tiles[y][x]->sprite.setPosition(sf::Vector2f(
                    static_cast<float>(worldX * TILE_SIZE),
                    static_cast<float>(worldY * TILE_SIZE)
                ));

                sf::Vector2u textureSize = texture->getSize();
                if (textureSize.x > 0 && textureSize.y > 0) {
                    chunk->tiles[y][x]->sprite.setScale(sf::Vector2f(
                        static_cast<float>(TILE_SIZE) / textureSize.x,
                        static_cast<float>(TILE_SIZE) / textureSize.y
                    ));
                }
            }

            // Cache collision data
            chunk->solidTiles[y][x] = (tileType == TileType::WATER || tileType == TileType::TREE);
        }
    }

    chunk->isLoaded = true;
    loadedChunks[chunkCoord] = std::move(chunk);
}

void Map::unloadDistantChunks(sf::Vector2f playerPos) {
    ChunkCoord playerChunk = {
        static_cast<int>(playerPos.x / (CHUNK_SIZE * TILE_SIZE)),
        static_cast<int>(playerPos.y / (CHUNK_SIZE * TILE_SIZE))
    };

    auto it = loadedChunks.begin();
    while (it != loadedChunks.end()) {
        ChunkCoord chunkCoord = it->first;
        int distanceX = std::abs(chunkCoord.x - playerChunk.x);
        int distanceY = std::abs(chunkCoord.y - playerChunk.y);

        if (distanceX > RENDER_DISTANCE + 1 || distanceY > RENDER_DISTANCE + 1) {
            it = loadedChunks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Map::loadChunksAroundPlayer(sf::Vector2f playerPos) {
    ChunkCoord playerChunk = {
        static_cast<int>(playerPos.x / (CHUNK_SIZE * TILE_SIZE)),
        static_cast<int>(playerPos.y / (CHUNK_SIZE * TILE_SIZE))
    };

    // Load only 1 chunk per frame to avoid stuttering
    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; dx++) {
        for (int dy = -RENDER_DISTANCE; dy <= RENDER_DISTANCE; dy++) {
            int x = playerChunk.x + dx;
            int y = playerChunk.y + dy;

            if (x >= 0 && x < CHUNKS_X && y >= 0 && y < CHUNKS_Y) {
                ChunkCoord coord = { x, y };
                if (loadedChunks.find(coord) == loadedChunks.end()) {
                    loadChunk(coord);
                    return; // Load only one chunk per frame
                }
            }
        }
    }
}

bool Map::isTileSolid(int worldX, int worldY) const {
    if (worldX < 0 || worldX >= WORLD_WIDTH || worldY < 0 || worldY >= WORLD_HEIGHT) {
        return true;
    }

    ChunkCoord chunkCoord = {
        worldX / CHUNK_SIZE,
        worldY / CHUNK_SIZE
    };

    auto chunkIt = loadedChunks.find(chunkCoord);
    if (chunkIt == loadedChunks.end()) {
        return false;
    }

    int tileX = worldX % CHUNK_SIZE;
    int tileY = worldY % CHUNK_SIZE;

    return chunkIt->second->solidTiles[tileY][tileX];
}

void Map::draw(sf::RenderWindow& window, sf::View& camera) {
    sf::Vector2f cameraCenter = camera.getCenter();
    sf::Vector2f cameraSize = camera.getSize();

    // Calculate visible area with some padding
    int startX = std::max(0, static_cast<int>((cameraCenter.x - cameraSize.x / 2) / TILE_SIZE) - 2);
    int endX = std::min(WORLD_WIDTH, static_cast<int>((cameraCenter.x + cameraSize.x / 2) / TILE_SIZE) + 2);
    int startY = std::max(0, static_cast<int>((cameraCenter.y - cameraSize.y / 2) / TILE_SIZE) - 2);
    int endY = std::min(WORLD_HEIGHT, static_cast<int>((cameraCenter.y + cameraSize.y / 2) / TILE_SIZE) + 2);

    // Draw visible chunks only
    for (const auto& chunkPair : loadedChunks) {
        const auto& chunk = chunkPair.second;

        // Check if chunk is visible
        int chunkStartX = chunk->coord.x * CHUNK_SIZE;
        int chunkEndX = chunkStartX + CHUNK_SIZE;
        int chunkStartY = chunk->coord.y * CHUNK_SIZE;
        int chunkEndY = chunkStartY + CHUNK_SIZE;

        if (chunkEndX < startX || chunkStartX > endX ||
            chunkEndY < startY || chunkStartY > endY) {
            continue; // Skip invisible chunks
        }

        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                int worldX = chunkStartX + x;
                int worldY = chunkStartY + y;

                if (worldX >= startX && worldX < endX && worldY >= startY && worldY < endY) {
                    if (useSimpleGraphics) {
                        // Draw simple rectangles for better performance
                        TileType tileType = generateTileType(worldX, worldY);
                        sf::RectangleShape* shape = nullptr;

                        switch (tileType) {
                        case TileType::WATER: shape = &waterTile; break;
                        case TileType::STONE: shape = &stoneTile; break;
                        case TileType::TREE: shape = &treeTile; break;
                        default: shape = &grassTile; break;
                        }

                        shape->setPosition(sf::Vector2f(
                            static_cast<float>(worldX * TILE_SIZE),
                            static_cast<float>(worldY * TILE_SIZE)
                        ));

                        window.draw(*shape);
                    }
                    else {
                        if (chunk->tiles[y][x].has_value()) {
                            window.draw(chunk->tiles[y][x]->sprite);
                        }
                    }
                }
            }
        }
    }
}