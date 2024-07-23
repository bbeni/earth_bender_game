#ifndef RESOURCES_H
#define RESOURCES_H
#include "glad/glad.h"
#include <cstdint>

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
		Texture earth_bender_color;
		Texture stone_block_color;
		Texture stone_tile_color;
		Texture lava_tile_color;
	} names;

	Texture as_array[sizeof(names)/sizeof(Texture)];
};

// load all textures defined below
bool init_texture_catalog_disk_and_gpu();

// access the textures
extern Texture_Catalog g_texture_catalog;

#endif // RESOURCES_H