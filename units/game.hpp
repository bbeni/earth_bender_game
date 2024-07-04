#pragma once

#include "mathematics.hpp"
#include "rendering_backend.hpp"

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
void draw_map_floor(Floor* floor);


// temporary for testing rendering the cube
extern Model_Info_For_Shading model_info;
void init_model_for_drawing();
void draw_model();