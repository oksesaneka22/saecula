#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Map settings
const int TILE_SIZE = 64;
const int WORLD_WIDTH = 2000;
const int WORLD_HEIGHT = 2000;

// Chunk system
const int CHUNK_SIZE = 16;
const int CHUNKS_X = WORLD_WIDTH / CHUNK_SIZE;
const int CHUNKS_Y = WORLD_HEIGHT / CHUNK_SIZE;
const int RENDER_DISTANCE = 8;

// Enums
enum class TileType {
    GRASS = 0,
    WATER = 1,
    STONE = 2,
    TREE = 3,
    DIRT = 4  // New dirt tile type
};

enum class BiomeType {
    GRASSLAND = 0,
    FOREST = 1,
    MOUNTAIN = 2,
    LAKE = 3,
    RIVER = 4
};

// Item types for inventory
enum class ItemType {
    GRASS = 0,
    WATER = 1,
    STONE = 2,
    TREE = 3,
    WOOD = 4,
    WOOD_PICKAXE = 5,  // New tool items
    WOOD_AXE = 6
};

// Tool slot types
enum class ToolSlotType {
    PICKAXE = 0,
    AXE = 1,
    TOOL_SLOT_COUNT = 2
};

#endif
