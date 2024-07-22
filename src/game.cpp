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
				t->height = i;
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
					t->height = 1;
					t->ramp_direction = Orientation::SOUTH;
				}
				else if (j == 8) {
					t->type = Tile_Type::STONE;
					t->height = 1;
					t->ramp_direction = Orientation::NORTH;
				}
				else {
					t->type = Tile_Type::GRASS;
				}
			}
		}
	}

	floor->tiles[3][10].ramp_direction = Orientation::EAST;
	floor->tiles[3][10].height += 1;
	floor->tiles[4][10].ramp_direction = Orientation::EAST;

	floor->tiles[19][10].ramp_direction = Orientation::WEST;
	floor->tiles[19][10].height += 1;
	floor->tiles[18][10].ramp_direction = Orientation::WEST;


}


Vec4 color_from_tile_type(Tile_Type type) {
	switch (type) {
	case Tile_Type::AIR :
		return Vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
	case Tile_Type::GRASS:
		return Vec4{ 0.0f, 0.95f, 0.1f, 1.0f };
	case Tile_Type::EARTH:
		return Vec4{ 0.75f, 0.5f, 0.6f, 1.0f };
	case Tile_Type::SAND:
		return Vec4{ 0.9f, 0.9f, 0.2f, 1.0f };
	case Tile_Type::STONE:
		return Vec4{ 0.5f, 0.5f, 0.5f, 1.0f };
	case Tile_Type::LAVA:
		return Vec4{ 0.99f, 0.45f, 0.1f, 1.0f };
	case Tile_Type::WATER:
		return Vec4{ 0.0f, 0.2f, 0.7f, 0.2f };
	default:
		return Vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
	}
}

Animated_Model_Info_For_Shading player_model_info = { 0 };
Model_Info_For_Shading stone_model_info = { 0 };

Model_Info_For_Shading base_tile_model_info = { 0 };
Model_Info_For_Shading north_ramp_model_info = { 0 };
Model_Info_For_Shading east_ramp_model_info = { 0 };
Model_Info_For_Shading south_ramp_model_info = { 0 };
Model_Info_For_Shading west_ramp_model_info = { 0 };


void init_models_for_drawing() {

	//construct_cube_triangles(&player_model_info.model);
	//construct_normals(&player_model_info.model);

	stone_model_info.model.mesh = load_mesh_bada_file("../resources/3d_models/stone_block.bada");
	stone_model_info.texture_color_path = (char*)"../resources/3d_models/stone_block_color.jpg";
	shader_init_model(&shader_brdf, &stone_model_info);

	
	//player_model_info.model.mesh = load_mesh_bada_file("../resources/3d_models/earth_bender.bada");
	//player_model_info.texture_color_path = (char*)"../resources/3d_models/earth_bender_color.jpg";
	//shader_init_model(&shader_brdf, &player_model_info);

	player_model_info.model = load_anim_bada_file("../resources/3d_models/earth_bender_anim.bada");
	player_model_info.texture_color_path = (char*)"../resources/3d_models/earth_bender_color.jpg";
	shader_init_animated_model(&shader_brdf, &player_model_info);

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

void draw_stone(Player *p) {
	Mat4 model_rotation = matrix_from_basis_vectors({ 1,0,0 }, { 0,1,0 }, { 0,0,1 });
	Mat4 translation = matrix_translation(Vec3{ 4, 4, 1.0f });

	shader_uniform_set(shader_brdf.gl_id, "model", translation * model_rotation * matrix_scale(0.5f));
	shader_draw_call(&stone_model_info);
}

void draw_floor(Floor* floor) {

	Mat4 model_rotation = matrix_from_basis_vectors({ 1,0,0 }, { 0,1,0 }, { 0,0,1 });
	Mat4 translation = matrix_translation(Vec3{ 2.0f, 1.0f, 5.0f });

	for (int i = 0; i < FLOOR_W; i++) {
		for (int j = 0; j < FLOOR_D; j++) {

			Tile tile = floor->tiles[i][j];

			if (tile.type == Tile_Type::AIR) continue;

			float elevation = 0.5f * (tile.height - (int)(tile.ramp_direction != Orientation::NO_ORIENTATION) );

			Vec4 color =color_from_tile_type(tile.type);
			shader_uniform_set(shader_phong.gl_id, "object_color", Vec3{ color.x, color.y, color.z });

			translation = matrix_translation(Vec3{ 1.0f * i, 1.0f * j, elevation});
			Mat4 model = translation * model_rotation;
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
				float ambient_strength = 0.05f;
				shader_uniform_set(shader_phong.gl_id, "ambient_strength", ambient_strength);
			}

		}
	}
}

void set_player_model_view_projection(Player* p) {

	// Model
	Mat4 model = matrix_translation(Vec3{ p->pos.x, p->pos.y, p->pos.z }) *
		matrix_rotation_euler(0.0f, 0.0f, (-p->direction_angle + M_PI)) *
		matrix_scale(0.8f);
	shader_uniform_set(shader_brdf.gl_id, "model", model);

	// View
	Vec3 camera_pos = Vec3{ p->pos.x, p->pos.y, 0.0f } + Vec3{ -3.5f, -3.5f, 7.5f };
	Mat4 view = matrix_camera(camera_pos, {1.5f, 1.5f, -2.0f}, {0.0f, 0.0f, 1.0f});
	float det = matrix_det(view);
	shader_uniform_set(shader_brdf.gl_id, "view", view);
	shader_uniform_set(shader_phong.gl_id, "view", view);

	// Projection
	float near_plane = 0.01f;
	float far_plane = 100.0f;
	Mat4 projection = matrix_perspective(p->fov, 1.4f, near_plane, far_plane);
	shader_uniform_set(shader_brdf.gl_id, "projection", projection);
	shader_uniform_set(shader_phong.gl_id, "projection", projection);
}


void draw_player(Player* p) {

	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.15f);

	set_player_model_view_projection(p);

	const float animation_frame_time = 0.025f;
	int frame_index = (int32_t)(get_time() / animation_frame_time) % player_model_info.model.count;

	if (p->current_action == Action::IDLE) {
		frame_index = 10;
	}

	shader_draw_call(&player_model_info, frame_index);

	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.05f);
}

void update_player(Player* p, Floor* floor) {

	float frame_time = (float)get_frame_time();

	float angle = p->direction_angle;
	float target_angle = p->target_direction_angle - 0.001f;


	if (fabs(angle - target_angle) > 0.00001f) {
		p->current_action = Action::TURNING;
		move_towards_on_circle(&p->direction_angle, target_angle, p->turn_speed, frame_time);
	}

	Vec3 direction = Vec3{ sinf(angle), cosf(angle) };


	if (p->current_action == Action::WALKING) {
		//move_towards(&p->fov, 72.0f, 90.0f, frame_time);
	} else {
		move_towards(&p->fov, 60.0f, 70.0f, frame_time);
	}

	p->velocity = direction * p->walk_speed;


	if (p->current_action == Action::WALKING) {
		Vec3 desired_pos = p->pos + p->velocity * frame_time;

		int current_i = (int)floorf(p->pos.x + 0.5f);
		int current_j = (int)floorf(p->pos.y + 0.5f);

		int desired_i = (int)floorf(desired_pos.x + 0.5f);
		int desired_j = (int)floorf(desired_pos.y + 0.5f);

		Tile* desired_tile = NULL;
		Tile* current_tile = NULL;
		{
			if ((desired_i >= 0 && desired_i < FLOOR_W) && (desired_j >= 0 && desired_j < FLOOR_D)) {
				desired_tile = &floor->tiles[desired_i][desired_j];
			}

			if ((current_i >= 0 && current_i < FLOOR_W) && (current_j >= 0 && current_j < FLOOR_D)) {
				current_tile = &floor->tiles[current_i][current_j];
			}
		}

		// snap the x or y cordinate to previous tiles
		if (current_tile != NULL && (desired_tile == NULL || desired_tile->block_walking)) {
			int d_i = desired_i - current_i;
			int d_j = desired_j - current_j;

			if (d_i != 0) {
				desired_pos.x = p->pos.x;
			}

			if (d_j != 0) {
				desired_pos.y = p->pos.y;
			}

			desired_pos.z = current_tile->height * 0.5f + 0.5f;
		}
		
		if (desired_tile != NULL && !desired_tile->block_walking) {

			desired_pos.z = desired_tile->height * 0.5f + 0.5f;

			if (desired_tile->ramp_direction != Orientation::NO_ORIENTATION) {

				float x = desired_pos.x - floorf(desired_pos.x + 0.5f) + 0.5f;
				float y = desired_pos.y - floorf(desired_pos.y + 0.5f) + 0.5f;

				switch (desired_tile->ramp_direction)
				{
				case Orientation::EAST:
					desired_pos.z += lerp(-0.5f, 0.0f, x);
					break;
				case Orientation::WEST:
					desired_pos.z += lerp(0.0f, -0.5f, x);
					break;
				case Orientation::NORTH:
					desired_pos.z += lerp(-0.5f, 0.0f, y);
					break;
				case Orientation::SOUTH:
					desired_pos.z += lerp(0.0f, -0.5f, y);
					break;
				default:
					break;
				}
			}
		}

		p->pos = desired_pos;
	}
}



void draw_minimap(Floor* floor, Player* p) {

	float offset = -1.0f; // rendering left bottom corner is (-1, -1)

	float width = 0.008f;
	float height = 0.012f;

	float pad_x = 0;// width * 0.013f;
	float pad_y = 0;//height * 0.013f;

	float x0 = width * 2.5f;
	float y0 = height * 2.5f;

	Vec2 player_pos = { x0 + offset + (width + pad_x) * p->pos.x + 0.5f * width, y0 + offset + (height + pad_y) * p->pos.y + 0.5f * height };

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
	immediate_quad(offset, offset, (width + pad_x) * FLOOR_W - pad_x + 2 * x0, (height + pad_y) * FLOOR_D - pad_y + 2 * y0, Vec4{ 0, 0, 0, 1 });

}