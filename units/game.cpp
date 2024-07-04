#include "game.hpp"
#include "rendering_backend.hpp"
#include "shaders.hpp"


void generate_floor(Floor* floor) {
	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {
			
			if ((i > 3 && i < 17) && (j > 3 && j < 17)) {

				if (j == 10) {
					floor->tiles[i][j].type = Tile_Type::STONE;
					floor->tiles[i][j].height = 2;
				}
				else {
					floor->tiles[i][j].type = Tile_Type::WATER;
				}
			}
			else {
				if (j == 10) {
					floor->tiles[i][j].type = Tile_Type::SAND;
					floor->tiles[i][j].height = 1;
				}
				else {
					floor->tiles[i][j].type = Tile_Type::GRASS;
				}
			}
		}
	}
}


Vec4 color_from_tile_type(Tile_Type type) {
	switch (type) {
	case Tile_Type::AIR :
		return Vec4{ 0.0, 0.0, 0.0, 0.0 };
	case Tile_Type::GRASS:
		return Vec4{ 0.0, 0.95, 0.1, 1.0 };
	case Tile_Type::EARTH:
		return Vec4{ 0.75, 0.5, 0.6, 1.0 };
	case Tile_Type::SAND:
		return Vec4{ 0.9, 0.9, 0.2, 1.0 };
	case Tile_Type::STONE:
		return Vec4{ 0.5, 0.5, 0.5, 1.0 };
	case Tile_Type::LAVA:
		return Vec4{ 0.65, 0.3, 0.0, 1.0 };
	case Tile_Type::WATER:
		return Vec4{ 0.0, 0.2, 0.7, 1.0 };
	default:
		return Vec4{ 1.0, 0, 0, 1.0 };
	}
}

Model_Info_For_Shading model_info = { 0 };

void init_model_for_drawing() {

	construct_cube_triangles(&model_info.model);
	construct_normals(&model_info.model);
	shader_init_model(&shader_phong, &model_info);
}

void draw_model() {
	shader_draw_call(&model_info);
}

void draw_map_floor(Floor* floor) {

	float width = 0.02f;
	float height = 0.03f;

	float pad_x = width * 0.1f;
	float pad_y = height * 0.1f;

	float x0 = width*0.5f - 1.0f;
	float y0 = height*0.5f - 1.0f;


	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {
			
			Vec2 pos = { x0 + (width + pad_x) * i + 0.5f * width, y0 + (height + pad_y) * j + 0.5f * height };
			
			//immediate_quad(pos, size, color_from_tile_type(floor->tiles[i][j].type));

			Vec4 c1 = color_from_tile_type(floor->tiles[i][j].type);
			Vec4 c2 = c1;
			c2.x -= 0.4f;
			c2.y -= 0.4f;
			c2.z -= 0.4f;
			clamp(&c2.x, 0, 1);
			clamp(&c2.y, 0, 1);
			clamp(&c2.z, 0, 1);

			Vec4 c3 = c2;
			c3.x += 0.3f;
			c3.y += 0.3f;
			c3.z += 0.3f;
			clamp(&c3.x, 0, 1);
			clamp(&c3.y, 0, 1);
			clamp(&c3.z, 0, 1);

			Vec2 p1 = { pos.x - width * 0.5f, pos.y - height * 0.5f };
			Vec2 p2 = { pos.x + width * 0.5f, pos.y - height * 0.5f };
			Vec2 p3 = { pos.x + width * 0.5f, pos.y + height * 0.5f };
			Vec2 p4 = { pos.x - width * 0.5f, pos.y + height * 0.5f };

			immediate_quad(p1, p2, p3, p4, c1, c1, c2, c3);

		}
	}
}

