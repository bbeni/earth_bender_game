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
	bool block_walking;
	Orientation ramp_direction;
};

#define FLOOR_W 40
#define FLOOR_D 40

typedef struct Floor {
	Tile tiles[FLOOR_D][FLOOR_W];
} Floor;

enum class Action {
	IDLE,
	TURNING,
	WALKING,
};

typedef struct Player {
	Vec3  pos;
	float direction_angle;
	Vec3  velocity;

	float target_direction_angle;
	Action current_action;

	// settings
	float walk_speed;
	float turn_speed;
	float fov;

} Player;

void generate_floor(Floor* floor);

void update_player(Player* p, Floor* floor);

void draw_map_floor(Floor* floor, Player* p);
void draw_floor(Floor* floor);
void draw_player(Player* p);



// temporary for testing rendering the cube
extern Model_Info_For_Shading base_tile_model_info;
void init_models_for_drawing();