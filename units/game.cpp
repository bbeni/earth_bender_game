#include "game.hpp"
#include "rendering_backend.hpp"
#include "shaders.hpp"


void generate_floor(Floor* floor) {
	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {

			if (i*i + j*j > 1400) {
				floor->tiles[i][j].type = Tile_Type::AIR;
				continue;
			}
			
			if ((i > 3 && i < 19) && (j > 3 && j < 19)) {

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
		return Vec4{ 0.0, 0.2, 0.7, 0.2 };
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

void draw_floor(Floor* floor) {

	srand((int)(get_time()*0.1f ));

	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {

			Tile tile = floor->tiles[i][j];

			if (tile.type == Tile_Type::AIR) continue;

			float random_elevation = -(float)((rand() % 12) == 0) + (float)((rand() % 20) == 0);
			
			Vec4 color =color_from_tile_type(tile.type);
			shader_uniform_set(shader_phong.gl_id, "object_color", Vec3{ color.x, color.y, color.z });

			Mat4 translation = matrix_translation(Vec3{ 1.0f * i, random_elevation, 1.0f * j});
			Mat4 model = matrix_scale(0.99f) * translation;
			shader_uniform_set(shader_phong.gl_id, "model", model);
			shader_draw_call(&model_info);
		}
	}
}

void draw_map_floor(Floor* floor, Player *p) {

	float offset = -1.0f; // rendering left bottom corner is (-1, -1)

	float width = 0.012f;
	float height = 0.018f;

	float pad_x = 0;// width * 0.013f;
	float pad_y = 0;//height * 0.013f;

	float x0 = width * 2.5f;
	float y0 = height * 2.5f;

	Vec2 player_pos = { x0 + offset + (width + pad_x) * p->pos.x + 0.5f * width, y0 + offset + (height + pad_y) * p->pos.z + 0.5f * height };

	immediate_quad(player_pos, Vec2{ width, height }, Vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

	Vec2 pos;

	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {
			
			pos = { x0 + offset + (width + pad_x) * i + 0.5f * width, y0 + offset + (height + pad_y) * j + 0.5f * height };
			
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
	immediate_quad(offset, offset, (width + pad_x) * FLOOR_W - pad_x + 2*x0, (height + pad_y) * FLOOR_D - pad_y + 2*y0, Vec4{ 0, 0, 0, 1 });

}


void update_player(Player* p) {
	float frame_time = get_frame_time();

	/*
	if (p->desired_direction.x != p->direction.x || p->desired_direction.y != p->direction.y) {
		move_towards(&p->direction, p->desired_direction, p->turn_speed, frame_time);
		normalize(&p->direction);
	}*/

	normalize(&p->direction);


	switch (p->current_action) {
	case Action::WALK_FORWARDS:
		p->velocity = p->direction;
		break;
	case Action::WALK_BACKWARDS:
		p->velocity = p->direction * -1;
		break;
	case Action::WALK_RIGHT:
		p->velocity = cross(p->direction, Vec3{ 0.0f, 1.0f, 0.0f });
		break;
	case Action::WALK_LEFT:
		p->velocity = cross(p->direction, Vec3{ 0.0f, -1.0f, 0.0f });
		break;
	}

	if (p->current_action == Action::WALK_FORWARDS) {
		move_towards(&p->fov, 110.0f, 290.0f, frame_time);
	} else {
		move_towards(&p->fov, 45.0f, 190.0f, frame_time);
	}
	float near_plane = 0.01f;
	float far_plane = 1000.0f;
	Mat4 projection = matrix_perspective(p->fov, 1.4f, near_plane, far_plane);
	shader_uniform_set(shader_phong.gl_id, "projection", projection);


	p->velocity = p->velocity * p->walk_speed;

	if (p->current_action != Action::IDLE) {
		p->pos = p->pos + p->velocity * get_frame_time();
	}

	p->pos.y = 0;


	Vec3 camera_direction = p->direction;

	//camera_direction.x -= 0.5f;
	//Scamera_direction.z -= 0.5f;

	normalize(&camera_direction);

	Vec3 camera_position = p->pos - camera_direction * 1.5f;
	camera_position.y = 1.8f;

	//Mat4 view = matrix_camera(camera_position, camera_direction, Vec3{ 0.0f, 1.0f * cosf(get_time()), 1.0f * sinf(get_time())});
	Mat4 view = matrix_camera(camera_position, camera_direction, Vec3{ 0.0f, 1.0f, 0.0f});
	shader_uniform_set(shader_phong.gl_id, "view", view);
}
