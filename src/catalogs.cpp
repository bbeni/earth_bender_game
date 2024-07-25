#include "catalogs.hpp"
#include "resource_loading.hpp"
#include "rendering_backend.hpp"

Texture_Catalog g_texture_catalog;
Model_Catalog g_model_catalog;

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

bool init_texture_catalog_disk_and_gpu() {
	g_texture_catalog.names.default_color = set("../resources/textures/default_color.jpg");
	g_texture_catalog.names.earth_bender_color = set("../resources/textures/earth_bender_color.jpg");
	g_texture_catalog.names.stone_block_color = set("../resources/textures/stone_block_color.jpg");
	g_texture_catalog.names.stone_tile_color = set("../resources/textures/stone_tile_color.jpg");
	g_texture_catalog.names.lava_tile_color = set("../resources/textures/lava_tile_color.jpg");
	g_texture_catalog.names.water_tile_color = set("../resources/textures/water_tile_color.jpg");

	g_texture_catalog.names.fluffy_monster_color = set("../resources/textures/fluffy_monster_color.jpg");

	// check if anyone failed
	for (int i = 0; i < sizeof(g_texture_catalog.as_array) / sizeof(g_texture_catalog.as_array[0]); i++) {
		if (!g_texture_catalog.as_array[i].loaded_on_gpu) {
			return false;
		}
	}
	return true;
}

Static_Model load_model(const char* file_path) {
	Static_Model m = { 0 };
	m.mesh = load_mesh_bada_file(file_path);
	return m;
}

bool init_model_catalog_disk() {
	g_model_catalog.names.stone_block = load_model("../resources/3d_models/stone_block.bada");
	g_model_catalog.names.stone_tile = load_model("../resources/3d_models/stone_tile.bada");
	g_model_catalog.names.stone_tile_ramp = load_model("../resources/3d_models/stone_tile_ramp.bada");
	g_model_catalog.names.stone_tile_ramp_special = load_model("../resources/3d_models/stone_tile_ramp_special.bada");
	g_model_catalog.names.water_tile = load_model("../resources/3d_models/water_tile.bada");
	g_model_catalog.names.fluffy_monster = load_model("../resources/3d_models/fluffy_monster.bada");

	// check if anyone failed
	for (int i = 0; i < sizeof(g_model_catalog.as_array) / sizeof(g_model_catalog.as_array[0]); i++) {
		if (g_model_catalog.as_array[i].mesh.count == 0) {
			printf("Error: the model with index '%d' failed to load\n", i);
			return false;
		}
	}
	return true;
}

