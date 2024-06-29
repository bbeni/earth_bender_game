#include "game.hpp"
#include "rendering_backend.hpp"

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


void draw_floor(Floor* floor) {
	Vec2 size = { 1.0 / 21, 1.0 / 21 };

	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {
			Vec2 pos = { (float)i / 10 - 1.0f + size.x, (float)j / 10 - 1.0f + size.y};
			immediate_quad(pos, size, color_from_tile_type(floor->tiles[i][j].type));

		}
	}
}

