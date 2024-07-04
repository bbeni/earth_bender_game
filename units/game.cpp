#include "game.hpp"
#include "rendering_backend.hpp"
#include "shaders.hpp"


void generate_floor(Floor* floor) {
	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {

			Tile* t = &floor->tiles[i][j];

			if (i*i + j*j > 1800) {
				t->type = Tile_Type::AIR;
				t->block_walking = true;
				continue;
			}

			// hole
			if ((i > 20) && (j > 5) && (i < 25) && (j < 13)) {
				t->type = Tile_Type::LAVA;
				t->block_walking = true;
				continue;

			}

			if ((j > 23) && (i > 5) && (j < 29) && (i < 13)) {
				t->type = Tile_Type::EARTH;
				//t->block_walking = true;
				t->height = 5;
				continue;
			}

			if ((i >= 1) && (i <= 6) && (j == 25)) {
				t->type = Tile_Type::EARTH;
				t->ramp_direction = Orientation::EAST;
				t->height = i - 1;
				continue;
			}

			if ((i > 3 && i < 19) && (j > 3 && j < 19)) {

				if (j == 10) {
					t->type = Tile_Type::STONE;
					t->height = 3;
				}

				else {
					t->type = Tile_Type::WATER;
					t->block_walking = true;
				}
			}
			else {
				if (j == 9 || j == 10 || j == 11) {
					t->type = Tile_Type::SAND;
					t->height = 1;
				}
				else if (j == 12) {
					t->type = Tile_Type::STONE;
					t->height = 0;
					t->ramp_direction = Orientation::SOUTH;
				}
				else if (j == 8) {
					t->type = Tile_Type::STONE;
					t->height = 0;
					t->ramp_direction = Orientation::NORTH;
				}
				else {
					t->type = Tile_Type::GRASS;
				}
			}
		}
	}

	floor->tiles[3][10].ramp_direction = Orientation::EAST;
	floor->tiles[4][10].ramp_direction = Orientation::EAST;
	floor->tiles[4][10].height -= 1;

	floor->tiles[19][10].ramp_direction = Orientation::WEST;
	floor->tiles[18][10].ramp_direction = Orientation::WEST;
	floor->tiles[18][10].height -= 1;


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
		return Vec4{ 0.99, 0.45, 0.1, 1.0 };
	case Tile_Type::WATER:
		return Vec4{ 0.0, 0.2, 0.7, 0.2 };
	default:
		return Vec4{ 1.0, 0, 0, 1.0 };
	}
}

Model_Info_For_Shading base_tile_model_info = { 0 };
Model_Info_For_Shading north_ramp_model_info = { 0 };
Model_Info_For_Shading east_ramp_model_info = { 0 };
Model_Info_For_Shading south_ramp_model_info = { 0 };
Model_Info_For_Shading west_ramp_model_info = { 0 };


void init_model_for_drawing() {

	construct_tile_triangles(&base_tile_model_info.model);
	construct_normals(&base_tile_model_info.model);
	shader_init_model(&shader_phong, &base_tile_model_info);

	construct_ramp_triangles(&north_ramp_model_info.model, Orientation::NORTH);
	construct_normals(&north_ramp_model_info.model);
	shader_init_model(&shader_phong, &north_ramp_model_info);

	construct_ramp_triangles(&south_ramp_model_info.model, Orientation::SOUTH);
	construct_normals(&south_ramp_model_info.model);
	shader_init_model(&shader_phong, &south_ramp_model_info);

	construct_ramp_triangles(&east_ramp_model_info.model, Orientation::EAST);
	construct_normals(&east_ramp_model_info.model);
	shader_init_model(&shader_phong, &east_ramp_model_info);

	construct_ramp_triangles(&west_ramp_model_info.model, Orientation::WEST);
	construct_normals(&west_ramp_model_info.model);
	shader_init_model(&shader_phong, &west_ramp_model_info);


}

void draw_floor(Floor* floor) {

	//srand((int)(get_time()*0.1f ));

	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {

			Tile tile = floor->tiles[i][j];

			if (tile.type == Tile_Type::AIR) continue;

			float elevation = 0.5f * tile.height;

			Vec4 color =color_from_tile_type(tile.type);
			shader_uniform_set(shader_phong.gl_id, "object_color", Vec3{ color.x, color.y, color.z });

			Mat4 translation = matrix_translation(Vec3{ 1.0f * i, elevation, 1.0f * j});
			Mat4 model = translation;
			shader_uniform_set(shader_phong.gl_id, "model", model);

			if (tile.type == Tile_Type::LAVA) {
				float ambient_strength = 0.9f;
				shader_uniform_set(shader_phong.gl_id, "ambient_strength", ambient_strength);
			}

			switch (tile.ramp_direction) {
			case Orientation::NORTH:
				shader_draw_call(&north_ramp_model_info); break;
			case Orientation::EAST:
				shader_draw_call(&east_ramp_model_info); break;
			case Orientation::SOUTH:
				shader_draw_call(&south_ramp_model_info); break;
			case Orientation::WEST:
				shader_draw_call(&west_ramp_model_info); break;
			default:
				shader_draw_call(&base_tile_model_info);
			}

			if (tile.type == Tile_Type::LAVA) {
				float ambient_strength = 0.25f;
				shader_uniform_set(shader_phong.gl_id, "ambient_strength", ambient_strength);
			}

		}
	}
}

void draw_player(Player* p) {

	Vec4 color = Vec4{1.0f, 0.0f, 0.0f, 1.0f};
	
	shader_uniform_set(shader_phong.gl_id, "object_color", Vec3{ color.x, color.y, color.z });

	Mat4 translation = matrix_translation(Vec3{ p->pos.x, p->pos.y + 2.0f, p->pos.z });
	Mat4 model = matrix_scale(Vec3{0.8f, 0.8f, 0.8f}) * translation;
	
	shader_uniform_set(shader_phong.gl_id, "model", model);

	shader_draw_call(&base_tile_model_info);
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


void update_player(Player* p, Floor* floor) {
	float frame_time = get_frame_time();

	/*
	if (p->desired_direction.x != p->direction.x || p->desired_direction.y != p->direction.y) {
		move_towards(&p->direction, p->desired_direction, p->turn_speed, frame_time);
		normalize(&p->direction);
	}*/

	normalize(&p->direction);

	if (p->current_action == Action::WALKING) {
		move_towards(&p->fov, 70.0f, 90.0f, frame_time);
	} else {
		move_towards(&p->fov, 60.0f, 90.0f, frame_time);
	}

	float near_plane = 0.01f;
	float far_plane = 1000.0f;
	Mat4 projection = matrix_perspective(p->fov, 1.4f, near_plane, far_plane);
	shader_uniform_set(shader_phong.gl_id, "projection", projection);

	p->velocity = p->direction * p->walk_speed;



	Vec3 desired_pos = p->pos;
	if (p->current_action != Action::IDLE) {
		desired_pos = desired_pos + p->velocity * get_frame_time();
	}

	Tile* desired_tile = NULL;
	{
		int i = (int)floorf(desired_pos.x + 0.5f);
		int j = (int)floorf(desired_pos.z + 0.5f);
		printf("ij: %d %d\n", i, j);

		if ((i >= 0 && i < FLOOR_W) && (j >= 0 && j < FLOOR_D)) {
			desired_tile = &floor->tiles[i][j];
		}
	}


	if (desired_tile != NULL && !desired_tile->block_walking) {
		p->pos = desired_pos;
		p->pos.y = 0.5f * desired_tile->height;
	}

	printf("Pos: %f %f\n", p->pos.x, p->pos.z);

	//p->pos = Vec3{ 0,0,0 };


	Vec3 camera_direction = Vec3{ 0.0f, -1.0f, 1.0f };
	normalize(&camera_direction);

	Vec3 camera_position = Vec3{ 0.0f, 10.0f, -7.0f };

	camera_position.x = camera_position.x - p->pos.x;   // TODO: fix x-axis is flipped. Also, we should prbably use z componoent for elevation. 
	camera_position.y = camera_position.y + p->pos.y;
	camera_position.z = camera_position.z + p->pos.z;


	//Mat4 view = matrix_camera(camera_position, camera_direction, Vec3{ 0.0f, 1.0f * cosf(get_time()), 1.0f * sinf(get_time())});
	Mat4 view = matrix_camera(camera_position, camera_direction, Vec3{ 0.0f, 0.0f, 1.0f});
	shader_uniform_set(shader_phong.gl_id, "view", view);
}
