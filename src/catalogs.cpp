#include "catalogs.hpp"
#include "resource_loading.hpp"
#include "rendering_backend.hpp"

Texture_Catalog g_texture_catalog;

// maybe use const char* everywhere?
inline Texture set(const char* file_path) {
	Texture tex = {};
	tex.file_path = (char *)file_path;
	tex.image = load_image_resource((char *)file_path);
	tex.loaded_on_disk = (bool)(tex.image.data != NULL);
	tex.gpu_handle = load_texture_gpu(&tex.image);
	tex.loaded_on_gpu = (bool)(tex.gpu_handle > 0);
	return tex;
}

bool check_all_initialized() {
	for (int i = 0; i < sizeof(g_texture_catalog.as_array) / sizeof(g_texture_catalog.as_array[0]); i++) {
		if (!g_texture_catalog.as_array[i].loaded_on_gpu) {
			return false;
		}
	}
	return true;
}

bool init_texture_catalog_disk_and_gpu() {
	g_texture_catalog.names.earth_bender_color = set("../resources/3d_models/earth_bender_color.jpg");
	g_texture_catalog.names.stone_block_color = set("../resources/3d_models/stone_block_color.jpg");
	g_texture_catalog.names.stone_tile_color = set("../resources/3d_models/stone_tile_color.jpg");
	g_texture_catalog.names.lava_tile_color = set("../resources/3d_models/lava_tile_color.jpg");

	return check_all_initialized();
}
