#ifndef CHUNK_H
#define CHUNK_H

#include <SFML/Graphics.hpp>
#include <optional>
#include "constants.h"

struct Tile {
    TileType type;
    sf::Sprite sprite;

    Tile() = default;
    Tile(TileType t, sf::Texture& texture) : type(t), sprite(texture) {}
};

struct ChunkCoord {
    int x, y;

    bool operator==(const ChunkCoord& other) const {
        return x == other.x && y == other.y;
    }
};

struct Chunk {
    ChunkCoord coord;
    std::optional<Tile> tiles[CHUNK_SIZE][CHUNK_SIZE];
    bool solidTiles[CHUNK_SIZE][CHUNK_SIZE];
    bool isLoaded = false;

    Chunk(ChunkCoord c) : coord(c) {
        // Initialize solid tiles to false
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                solidTiles[y][x] = false;
            }
        }
    }
};

#endif
