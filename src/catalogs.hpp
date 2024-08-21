#ifndef RESOURCES_H
#define RESOURCES_H
#include "glad/glad.h"
#include <cstdint>

#include "models.hpp"

struct Image {
	int w, h, n_components;
	unsigned char* data;
};

struct Texture {
	char* file_path;     // set by user before calling
	bool loaded_on_gpu;
	bool loaded_on_disk;
	Image image;
	GLuint gpu_handle;
};


// @Bug might be undefined bahviour, compiler dependent
union Texture_Catalog {
	struct {
		Texture default_color;
		Texture earth_bender_color;
		Texture stone_block_color;
		Texture stone_tile_color;
		Texture lava_tile_color;
		Texture water_tile_color;
		Texture fluffy_monster_color;
		Texture plain_red;
	} names;

	Texture as_array[sizeof(names)/sizeof(Texture)];
};

union Model_Catalog {
	struct {
		Static_Model marker_icosphere;
		Static_Model stone_block;
		Static_Model stone_tile;
		Static_Model stone_tile_ramp;
		Static_Model stone_tile_ramp_special;
		Static_Model water_tile;
		Static_Model fluffy_monster;
	} names ;

	Static_Model as_array[sizeof(names) / sizeof(Static_Model)];
};

// loading functions
bool init_texture_catalog_disk_and_gpu();
bool init_model_catalog_disk();

// access the textures
extern Texture_Catalog g_texture_catalog;
// access the models
extern Model_Catalog g_model_catalog;


#endif // RESOURCES_H