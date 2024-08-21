#include "game.hpp"
#include "rendering_backend.hpp"
#include "shaders.hpp"


void add_lower_tile(Level* level, Decoration_Tile tile) {
	if (level->n_lower_tiles >= sizeof(level->lower_tiles) / sizeof(level->lower_tiles[0])) {
		printf("Error: failed to add another Lower_Tile in add_lower_tile() %u exeeded.\n", sizeof(level->lower_tiles) / sizeof(level->lower_tiles[0]));
		return;
	}

	size_t i = level->n_lower_tiles++;
	level->lower_tiles[i] = tile;
}

void set_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation, Tile_Type type, Ramp_Orientation ramp) {
	assert(i < room->depth);
	assert(j < room->width);
	assert(elevation < room->height);

	TILE_AT(room, i, j, elevation).type = type;
	TILE_AT(room, i, j, elevation).ramp_direction = ramp;
}

void set_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation, Tile_Type type) {
	set_tile(room, i, j, elevation, type, Ramp_Orientation::FLAT);
}


void clear_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation) {
	set_tile(room, i, j, elevation, Tile_Type::AIR, Ramp_Orientation::FLAT);
}

void set_decoration_tile() {

}

Room room_alloc(uint32_t d, uint32_t w, uint32_t h) {
	Room r;
	r.depth = d;
	r.width = w;
	r.height = h;
	r.tiles = (Floor_Tile*)malloc(sizeof(*r.tiles) * d * w * h);
	memset(r.tiles, 0, sizeof(*r.tiles) * d * w * h);
	assert(r.tiles != NULL);
	return r;
}

void room_free(Room* room) {
	free(room->tiles);
	// @MemoryLeak
	// TODO: free also decoration tiles
}

void generate_level(Level* level, Room* room) {

	if (room->tiles == NULL) {
		*room = room_alloc(room->depth, room->width, room->height);
	}
	
	for (int i = 0; i < FLOOR_D; i++) {
		for (int j = 0; j < FLOOR_W; j++) {

			Top_Tile* t = &level->top_tiles[i][j];

			if (i*i + j*j > 1800) {
				t->type = Tile_Type::AIR;
				t->block_walking = true;
				continue;
			}

			// hole
			if ((i > 20) && (j > 5) && (i < 25) && (j < 13)) {
				t->type = Tile_Type::LAVA;
				t->block_walking = true;
				set_tile(room, i, j, 0, Tile_Type::LAVA);
				continue;
			}

			if ((j > 23) && (i > 5) && (j < 29) && (i < 13)) {
				t->type = Tile_Type::EARTH;
				t->height = 5;
				set_tile(room, i, j, 5, Tile_Type::EARTH);

				// add a lava tile below
				Decoration_Tile lt = { 0 };
				lt.type = Tile_Type::LAVA;
				lt.height = -1;
				lt.x = i;
				lt.y = j;				
				add_lower_tile(level, lt);
								
				set_tile(room, i, j, 0, Tile_Type::LAVA);

				continue;
			}

			if ((i >= 1) && (i <= 6) && (j == 25)) {
				t->type = Tile_Type::EARTH;
				t->ramp_direction = Ramp_Orientation::EAST;
				t->height = i;

				set_tile(room, i, j, i, Tile_Type::EARTH, Ramp_Orientation::EAST);


				// add a lava tile below
				Decoration_Tile lt = { 0 };
				lt.type = Tile_Type::LAVA;
				lt.height = 0;
				lt.x = i;
				lt.y = j;
				add_lower_tile(level, lt);

				set_tile(room, i, j, 0, Tile_Type::LAVA);

				continue;
			}

			if ((i > 3 && i < 19) && (j > 3 && j < 19)) {

				if (j == 10) {
					t->type = Tile_Type::STONE;
					t->height = 3;
					set_tile(room, i, j, 3, Tile_Type::STONE);

					// add a water tile below
					Decoration_Tile lt = { 0 };
					lt.type = Tile_Type::WATER;
					lt.height = 0;
					lt.x = i;
					lt.y = j;
					add_lower_tile(level, lt);

					set_tile(room, i, j, 0, Tile_Type::WATER);

				} else {
					t->type = Tile_Type::WATER;
					t->block_walking = true;
					t->height = 0;

					set_tile(room, i, j, 0, Tile_Type::WATER);
				}
			} else {
				if (j == 9 || j == 10 || j == 11) {
					t->type = Tile_Type::SAND;
					t->height = 1;
					set_tile(room, i, j, 1, Tile_Type::SAND);

				} else if (j == 12) {
					t->type = Tile_Type::STONE;
					t->height = 1;
					t->ramp_direction = Ramp_Orientation::SOUTH;

					set_tile(room, i, j, 1, Tile_Type::STONE, Ramp_Orientation::SOUTH);

				} else if (j == 8) {
					t->type = Tile_Type::STONE;
					t->height = 1;
					t->ramp_direction = Ramp_Orientation::NORTH;
					set_tile(room, i, j, 1, Tile_Type::STONE, Ramp_Orientation::NORTH);

				} else {
					t->type = Tile_Type::GRASS;
					set_tile(room, i, j, 0, Tile_Type::GRASS);
				}
			}
		}
	}

	level->top_tiles[3][10].ramp_direction = Ramp_Orientation::EAST;
	level->top_tiles[3][10].height += 1;
	set_tile(room, 3, 10, 2, Tile_Type::STONE, Ramp_Orientation::EAST);

	level->top_tiles[4][10].ramp_direction = Ramp_Orientation::EAST;
	set_tile(room, 4, 10, 3, Tile_Type::STONE, Ramp_Orientation::EAST);

	level->top_tiles[19][10].ramp_direction = Ramp_Orientation::WEST;
	level->top_tiles[19][10].height += 1;
	set_tile(room, 19, 10, 2, Tile_Type::STONE, Ramp_Orientation::WEST);

	level->top_tiles[18][10].ramp_direction = Ramp_Orientation::WEST;
	set_tile(room, 18, 10, 3, Tile_Type::STONE, Ramp_Orientation::WEST);

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
Loaded_Models loaded_models = { 0 };

Model_Info_For_Shading box_line_model = { 0 };

void init_models_for_drawing() {

	if (!init_texture_catalog_disk_and_gpu()) {
		printf("Error: failed to load some textures..\n");
		exit(1);
	}

	if (!init_model_catalog_disk()) {
		printf("Error: failed to load some models..\n");
		exit(1);
	}

	loaded_models.stone.model         = g_model_catalog.names.stone_block;
	loaded_models.stone.texture_color = &g_texture_catalog.names.stone_block_color;
	shader_init_model(&shader_brdf, &loaded_models.stone);

	loaded_models.stone_tile.model = g_model_catalog.names.stone_tile;
	loaded_models.stone_tile.texture_color = &g_texture_catalog.names.stone_tile_color;
	shader_init_model(&shader_brdf, &loaded_models.stone_tile);

	loaded_models.lava_tile.model = g_model_catalog.names.stone_tile;
	loaded_models.lava_tile.texture_color = &g_texture_catalog.names.lava_tile_color;
	shader_init_model(&shader_brdf, &loaded_models.lava_tile);

	loaded_models.water_tile.model = g_model_catalog.names.water_tile;
	loaded_models.water_tile.texture_color = &g_texture_catalog.names.water_tile_color;
	shader_init_model(&shader_water, &loaded_models.water_tile);

	loaded_models.stone_tile_ramp.model = g_model_catalog.names.stone_tile_ramp;
	loaded_models.stone_tile_ramp.texture_color = &g_texture_catalog.names.stone_tile_color;
	shader_init_model(&shader_brdf, &loaded_models.stone_tile_ramp);

	loaded_models.stone_tile_ramp.model = g_model_catalog.names.stone_tile_ramp;
	loaded_models.stone_tile_ramp.texture_color = &g_texture_catalog.names.stone_tile_color;
	shader_init_model(&shader_brdf, &loaded_models.stone_tile_ramp);

	loaded_models.stone_tile_ramp_special.model = g_model_catalog.names.stone_tile_ramp_special;
	loaded_models.stone_tile_ramp_special.texture_color = &g_texture_catalog.names.stone_tile_color;
	shader_init_model(&shader_brdf, &loaded_models.stone_tile_ramp_special);

	make_cube_model(&loaded_models.cube.model);
	loaded_models.cube.texture_color = &g_texture_catalog.names.default_color;
	shader_init_model(&shader_brdf, &loaded_models.cube);

	loaded_models.monster.model = g_model_catalog.names.fluffy_monster;
	loaded_models.monster.texture_color = &g_texture_catalog.names.fluffy_monster_color;
	shader_init_model(&shader_brdf, &loaded_models.monster);

	loaded_models.bender.model = load_model_bada_file("../resources/3d_models/earth_bender_anim.bada");
	loaded_models.bender.texture_color = &g_texture_catalog.names.earth_bender_color;
	shader_init_model(&shader_brdf, &loaded_models.bender);

	player_model_info.model = load_anim_bada_file("../resources/3d_models/earth_bender_anim.bada");
	player_model_info.texture_color = &g_texture_catalog.names.earth_bender_color;
	shader_init_animated_model(&shader_brdf, &player_model_info);

	box_line_model.model = g_model_catalog.names.stone_block;//construct_box_lines();
	shader_init_model(&shader_editor_box, &box_line_model);
}

void draw_stone(Bender *p) {
	Mat4 translation = matrix_translation(Vec3{ 5, 5, 0.15f });

	shader_uniform_set(shader_brdf.gl_id, "model", translation * matrix_rotation_euler(0.0f, 0.0f, get_time()));
	shader_draw_call(&loaded_models.stone);
}

void draw_tile(Tile_Type type, Ramp_Orientation ramp_direction, float elevation, int16_t x, int16_t y) {

	if (type == Tile_Type::AIR) return;


	auto model = &loaded_models.stone_tile;
	if (type == Tile_Type::LAVA) model = &loaded_models.lava_tile;
	if (type == Tile_Type::WATER) model = &loaded_models.water_tile;

	GLuint current_shader = model->shader->gl_id;

	if (type == Tile_Type::LAVA) {
		float ambient_strength = 0.9f;
		shader_uniform_set(current_shader, "ambient_strength", ambient_strength);
	}

	else if (type == Tile_Type::WATER) {
		float ambient_strength = 0.5f;
		shader_uniform_set(current_shader, "ambient_strength", ambient_strength);
	}

	Mat4 model_matrix = matrix_translation(Vec3{ 1.0f * x, 1.0f * y, elevation});
	shader_uniform_set(current_shader, "model", model_matrix);


	switch (ramp_direction) {
	case Ramp_Orientation::SOUTH:
		shader_draw_call(&loaded_models.stone_tile_ramp);
		break;
	case Ramp_Orientation::EAST:
		shader_uniform_set(current_shader, "model", model_matrix * model_rotation_90());
		shader_draw_call(&loaded_models.stone_tile_ramp);
		break;
	case Ramp_Orientation::NORTH:
		shader_uniform_set(current_shader, "model", model_matrix * model_rotation_180());
		shader_draw_call(&loaded_models.stone_tile_ramp);
		break;
	case Ramp_Orientation::WEST:
		shader_uniform_set(current_shader, "model", model_matrix * model_rotation_270());
		shader_draw_call(&loaded_models.stone_tile_ramp);
		break;
	default:
		shader_draw_call(model);
		break;
	}

	// reset the ambient_strength
	float ambient_strength = 0.05f;
	shader_uniform_set(current_shader, "ambient_strength", ambient_strength);

}

// note model needs to always be set manually
void update_gpu_for_shading(Bender *bender) {

	// View
	Vec3 camera_pos = Vec3{ bender->pos.x, bender->pos.y, 0.0f } + Vec3{ -3.5f, -3.5f, 7.5f };
	Mat4 view = matrix_camera(camera_pos, { 1.5f, 1.5f, -2.0f }, { 0.0f, 0.0f, 1.0f });
	float det = matrix_det(view);
	shader_uniform_set(shader_brdf.gl_id, "view", view);
	shader_uniform_set(shader_phong.gl_id, "view", view);
	shader_uniform_set(shader_water.gl_id, "view", view);
	shader_uniform_set(shader_editor_box.gl_id, "view", view);

	// Projection
	float near_plane = 0.01f;
	float far_plane = 100.0f;
	float aspect = bender->aspect;
	Mat4 projection = matrix_perspective_projection(bender->fov, aspect, near_plane, far_plane);
	
	if (bender->use_orthographic) {
		projection = matrix_orthographic_projection(-5.0f * aspect, +5.0f * aspect, 5.0f, -5.0f, near_plane, far_plane);
	}

	shader_uniform_set(shader_brdf.gl_id, "projection", projection);
	shader_uniform_set(shader_phong.gl_id, "projection", projection);
	shader_uniform_set(shader_water.gl_id, "projection", projection);
	shader_uniform_set(shader_editor_box.gl_id, "projection", projection);


	// globals
	shader_uniform_set(shader_water.gl_id, "time", get_time());

}

void draw_level(Room* room) {

	Mat4 translation = matrix_translation(Vec3{ 2.0f, 1.0f, 5.0f });

	for (int i = 0; i < room->depth; i++) {
		for (int j = 0; j < room->width; j++) {
			for (int k = 0; k < room->height; k++) {
				Floor_Tile tile = TILE_AT(room, i, j, k);
				if (tile.type != Tile_Type::AIR) {
					float elevation = 0.5f * k;
					draw_tile(tile.type, tile.ramp_direction, elevation, i, j);
				}
			}
		}
	}
}

void draw_player(Bender* p) {

	// Model
	Mat4 model = matrix_translation(Vec3{ p->pos.x, p->pos.y, p->pos.z }) *
		matrix_rotation_euler(0.0f, 0.0f, (-p->direction_angle + M_PI)) *
		matrix_scale(0.8f);
	shader_uniform_set(shader_brdf.gl_id, "model", model);


	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.15f);


	const float animation_frame_time = 0.025f;
	int frame_index = (int32_t)(get_time() / animation_frame_time) % player_model_info.model.count;

	if (p->current_action == Action::IDLE) {
		frame_index = 10;
	}

	shader_draw_call(&player_model_info, frame_index);

	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.05f);
}

void draw_game(Bender* bender, Room* room) {

	update_gpu_for_shading(bender);

	draw_player(bender);
	draw_level(room);
	draw_stone(bender);
}



void update_player(Bender* b, Level* floor) {

	float frame_time = (float)get_frame_time();

	float angle = b->direction_angle;
	float target_angle = b->target_direction_angle - 0.001f;


	if (fabs(angle - target_angle) > 0.00001f) {
		b->current_action = Action::TURNING;
		move_towards_on_circle(&b->direction_angle, target_angle, b->turn_speed, frame_time);
	}

	Vec3 direction = Vec3{ sinf(angle), cosf(angle) };

	b->velocity = direction * b->walk_speed;


	if (b->current_action == Action::WALKING) {
		Vec3 desired_pos = b->pos + b->velocity * frame_time;

		int current_i = (int)floorf(b->pos.x + 0.5f);
		int current_j = (int)floorf(b->pos.y + 0.5f);

		int desired_i = (int)floorf(desired_pos.x + 0.5f);
		int desired_j = (int)floorf(desired_pos.y + 0.5f);

		Top_Tile* desired_tile = NULL;
		Top_Tile* current_tile = NULL;
		{
			if ((desired_i >= 0 && desired_i < FLOOR_W) && (desired_j >= 0 && desired_j < FLOOR_D)) {
				desired_tile = &floor->top_tiles[desired_i][desired_j];
			}

			if ((current_i >= 0 && current_i < FLOOR_W) && (current_j >= 0 && current_j < FLOOR_D)) {
				current_tile = &floor->top_tiles[current_i][current_j];
			}
		}

		// snap the x or y cordinate to previous tiles
		if (current_tile != NULL && (desired_tile == NULL || desired_tile->block_walking)) {
			int d_i = desired_i - current_i;
			int d_j = desired_j - current_j;

			if (d_i != 0) {
				desired_pos.x = b->pos.x;
			}

			if (d_j != 0) {
				desired_pos.y = b->pos.y;
			}

			desired_pos.z = current_tile->height * 0.5f + 0.5f;
		}
		
		if (desired_tile != NULL && !desired_tile->block_walking) {

			desired_pos.z = desired_tile->height * 0.5f + 0.5f;

			if (desired_tile->ramp_direction != Ramp_Orientation::FLAT) {

				float x = desired_pos.x - floorf(desired_pos.x + 0.5f) + 0.5f;
				float y = desired_pos.y - floorf(desired_pos.y + 0.5f) + 0.5f;

				switch (desired_tile->ramp_direction)
				{
				case Ramp_Orientation::EAST:
					desired_pos.z += lerp(-0.5f, 0.0f, x);
					break;
				case Ramp_Orientation::WEST:
					desired_pos.z += lerp(0.0f, -0.5f, x);
					break;
				case Ramp_Orientation::NORTH:
					desired_pos.z += lerp(-0.5f, 0.0f, y);
					break;
				case Ramp_Orientation::SOUTH:
					desired_pos.z += lerp(0.0f, -0.5f, y);
					break;
				default:
					break;
				}
			}
		}

		b->pos = desired_pos;
	}
}

void draw_minimap(Room* room, Bender* p) {

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

			Tile_Type top_type = Tile_Type::AIR;
			for (int k = 0; k < room->height; k++) {
				top_type = TILE_AT(room, i, j, k).type;
				if (top_type != Tile_Type::AIR) {
					break;
				}
			}

			Vec4 c1 = color_from_tile_type(top_type);

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