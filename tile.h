#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include "constants.h"

class Tile {
public:
    sf::Sprite sprite;
    TileType type;

    Tile(TileType tileType, const sf::Texture& texture);
};

#endif