#include "chunk.h"

Chunk::Chunk(ChunkCoord chunkCoord) : coord(chunkCoord) {
    tiles.resize(CHUNK_SIZE);
    solidTiles.resize(CHUNK_SIZE);
    for (int y = 0; y < CHUNK_SIZE; y++) {
        tiles[y].resize(CHUNK_SIZE);
        solidTiles[y].resize(CHUNK_SIZE, false);
    }
}