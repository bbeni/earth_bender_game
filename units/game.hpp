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

#define FLOOR_W 40
#define FLOOR_D 40

typedef struct Floor {
	Tile tiles[FLOOR_D][FLOOR_W];
} Floor;

enum class Action {
	IDLE,
	WALK_FORWARDS,
	WALK_BACKWARDS,
	WALK_LEFT,
	WALK_RIGHT,
};

typedef struct Player {
	Vec3 pos;
	Vec3 direction;
	Vec3 velocity;

	Action current_action;

	// settings
	float walk_speed;
	float turn_speed;
	float fov;

} Player;

void generate_floor(Floor* floor);
void draw_map_floor(Floor* floor, Player* p);
void draw_floor(Floor* floor);


void update_player(Player* p);

// temporary for testing rendering the cube
extern Model_Info_For_Shading model_info;
void init_model_for_drawing();
