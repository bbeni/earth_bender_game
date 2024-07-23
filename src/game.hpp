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

struct Tile {
	int height;
	Tile_Type type;
	bool block_walking;
	Ramp_Orientation ramp_direction;
};

#define FLOOR_W 40
#define FLOOR_D 40

struct Level {
	Tile tiles[FLOOR_D][FLOOR_W];
};

enum class Action {
	IDLE,
	TURNING,
	WALKING,
};

struct Bender {
	Vec3  pos;
	float direction_angle;
	Vec3  velocity;

	float target_direction_angle;
	Action current_action;

	// settings
	float walk_speed;
	float turn_speed;
	float fov;

};

void generate_floor(Level* floor);

void update_player(Bender* p, Level* floor);

void draw_minimap(Level* floor, Bender* p);
void draw_floor(Level* floor);
void draw_player(Bender* p);
void draw_stone(Bender* p);


// temporary for testing rendering the cube
extern Model_Info_For_Shading base_tile_model_info;
void init_models_for_drawing();
