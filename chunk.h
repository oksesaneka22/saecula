#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <optional>
#include "tile.h"
#include "utils.h"
#include "constants.h"

class Chunk {
public:
    std::vector<std::vector<std::optional<Tile>>> tiles;
    ChunkCoord coord;
    bool isLoaded = false;

    // Cache for fast collision detection
    std::vector<std::vector<bool>> solidTiles;

    Chunk(ChunkCoord chunkCoord);
};

#endif