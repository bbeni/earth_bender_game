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

struct Top_Tile {
	int16_t   height;
	Tile_Type type;
	bool      block_walking;
	Ramp_Orientation ramp_direction;
};

// for now they are just aestetically here
struct Lower_Tile {
	int16_t   height;
	int16_t   x, y;
	Tile_Type type;
};

#define FLOOR_W 40
#define FLOOR_D 40

#define MAX_LOWER_TILES 40 * 40 * 2

struct Level {
	Top_Tile   top_tiles[FLOOR_D][FLOOR_W];
	Lower_Tile lower_tiles[MAX_LOWER_TILES];
	size_t     n_lower_tiles;
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
	float fov; // @Temporary
	float aspect; // @Temporary

	bool use_orthographic; // @Temporary
};


void generate_floor(Level* floor);

void update_player(Bender* p, Level* floor);

void draw_game(Bender* bender, Level* level);

void draw_minimap(Level* floor, Bender* p);
void draw_floor(Level* floor);
void draw_player(Bender* p);
void draw_stone(Bender* p);


// temporary for testing rendering the cube
extern Model_Info_For_Shading base_tile_model_info;
extern Model_Info_For_Shading box_line_model;

void init_models_for_drawing();

extern union Loaded_Models {
	struct {
		Model_Info_For_Shading stone_tile;
		Model_Info_For_Shading lava_tile;
		Model_Info_For_Shading water_tile;
		Model_Info_For_Shading stone_tile_ramp;
		Model_Info_For_Shading stone_tile_ramp_special;
		Model_Info_For_Shading cube;
		Model_Info_For_Shading stone;
		Model_Info_For_Shading bender;
		Model_Info_For_Shading monster;
	};

	Model_Info_For_Shading as_array[9]; // make sure this is updated when new model is inserted

} loaded_models;