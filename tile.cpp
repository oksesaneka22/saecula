#include "tile.h"

Tile::Tile(TileType tileType, const sf::Texture& texture)
    : type(tileType), sprite(texture) {
}