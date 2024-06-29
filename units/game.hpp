#pragma once

#include "mathematics.hpp"

enum class Tile_Type {
	AIR,
	GRASS,
	EARTH,
	SAND,
	STONE,
	LAVA,
	WATER,
};

typedef struct Tile {
	int height;
	Tile_Type type;
	bool walkable;
	bool is_ramp;
};

#define FLOOR_W 20
#define FLOOR_D 20

typedef struct Floor {
	Tile tiles[FLOOR_D][FLOOR_W];
} Floor;

void generate_floor(Floor* floor);
void draw_floor(Floor* floor);

