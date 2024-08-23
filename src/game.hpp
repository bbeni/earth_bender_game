#pragma once

#include "mathematics.hpp"
#include "rendering_backend.hpp"

enum class Tile_Type {
	AIR = 0,
	GRASS,
	EARTH,
	SAND,
	STONE,
	LAVA,
	WATER,
};

// for now they are just aestetically here
struct Decoration_Tile {
	int16_t   height;
	int16_t   x, y;
	Tile_Type type;
};

struct Floor_Tile {
	Tile_Type	 type;
	Ramp_Orientation ramp_direction;
	bool		 allow_walking;
};


// i depth, j width, k height direction
#define TILE_AT(room, i, j, k) (room)->tiles[(i)*(room)->width*(room)->height + (j)*(room)->height + k]

struct Room {
	size_t depth;
	size_t width;
	size_t height;

	Floor_Tile* tiles;

	struct {
		Box*   data;
		size_t count;
		size_t capacity;
	} tile_boxes;

};



// Stupid utility stuff
struct Index_Pos { uint32_t i, j, elevation; };
Vec3 position_conversion(uint32_t i, uint32_t j, uint32_t elevation);
Vec3 position_conversion(Index_Pos index_pos);
Index_Pos position_conversion(Vec3 pos);
bool tile_in_bounds(Room* room, uint32_t i, uint32_t j, uint32_t elevation);
bool tile_in_bounds(Room* room, Vec3 pos);

Room room_alloc(uint32_t d, uint32_t w, uint32_t h);
void room_free(Room* room);
void print_room(Room* room);
void set_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation, Tile_Type type, Ramp_Orientation ramp);
void set_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation, Tile_Type type);
void remove_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation);

Room generate_room_example(uint32_t depth, uint32_t width, uint32_t height);
Room generate_room_flat(uint32_t depth, uint32_t width, uint32_t height);


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

	bool jumping;
	float jump_vel;
	float jump_pos;
};


void update_player(Bender* p, Room* room);

void draw_game(Bender* bender, Room* room);

void draw_minimap(Room* room, Bender* p);
void draw_room(Room* room);
void draw_player(Bender* p);
void draw_stone(Bender* p);

Model_Info_For_Shading* model_info_from_type(Tile_Type type, Ramp_Orientation ramp_direction);


// temporary for testing rendering the cube
extern Model_Info_For_Shading base_tile_model_info;
extern Model_Info_For_Shading box_line_model;
extern Model_Info_For_Shading marker_round_model;

void init_models_for_drawing();

void draw_debug_sphere(Vec3 pos);
void draw_debug_box(Box box, Vec4 color);

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