#include "game.hpp"
#include "rendering_backend.hpp"
#include "shaders.hpp"



// forward conversion
Vec3 position_conversion(uint32_t i, uint32_t j, uint32_t elevation) {
	return Vec3{ (float)i,(float)j,(float)elevation * 0.5f };
}

// forward conversion
Vec3 position_conversion(Index_Pos index_pos) {
	return Vec3{ (float)index_pos.i, (float)index_pos.j, (float)index_pos.elevation * 0.5f };
}

// inverse conversion
Index_Pos position_conversion(Vec3 pos) {
	return Index_Pos{ (uint32_t)floorf(pos.x), (uint32_t)floorf(pos.y), (uint32_t)floorf(pos.y * 2.0f)};
}


bool tile_in_bounds(Room* room, uint32_t i, uint32_t j, uint32_t elevation) {
	return i < room->depth && j < room->width && elevation < room->height;
}

bool tile_in_bounds(Room* room, Vec3 pos) {
	Index_Pos ip = position_conversion(pos);
	return tile_in_bounds(room, ip.i, ip.j, ip.elevation);
}



void remove_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation) {
	assert(i < room->depth);
	assert(j < room->width);
	assert(elevation < room->height);

	auto type = TILE_AT(room, i, j, elevation).type;
	if (type == Tile_Type::AIR) return;

	auto ramp_dir = TILE_AT(room, i, j, elevation).ramp_direction;
	Vec3 pos = position_conversion(i, j, elevation);

	auto mi = model_info_from_type(type, ramp_dir);
	Box b = mi->model.bounding_box;
	b.max += pos;
	b.min += pos;

	set_tile(room, i, j, elevation, Tile_Type::AIR);

	// TODO: this is inconsistent see set_tile()
	for (int i = 0; i < room->tile_boxes.count; i++) {
		Box bi = room->tile_boxes.data[i];
		if (b.max == bi.max && b.min == bi.min) {
			array_unordered_remove(&room->tile_boxes, i);
			return;
		}
	}

}

void set_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation, Tile_Type type, Ramp_Orientation ramp) {
	if (!tile_in_bounds(room, i, j, elevation)) return;

	TILE_AT(room, i, j, elevation).type = type;
	TILE_AT(room, i, j, elevation).ramp_direction = ramp;
	TILE_AT(room, i, j, elevation).allow_walking = true;

	if (type == Tile_Type::AIR) return;

	Vec3 pos = position_conversion(i, j, elevation);

	auto mi = model_info_from_type(type, ramp);
	Box b = mi->model.bounding_box;
	b.max += pos;
	b.min += pos;

	// replace the one if we have already a collision box there 
	// TODO: this is inconsistent if we have a floating point deviation or smth
	// maybe better just link it to the actual tile!
	//  same problem above in remove_tile()
	for (int i = 0; i < room->tile_boxes.count; i++) {
		Box bi = room->tile_boxes.data[i];
		if (b.max == bi.max && b.min == bi.min) {
			room->tile_boxes.data[i] = b;
			return;
		}
	}

	array_add(&room->tile_boxes, b);
}

void set_tile(Room* room, uint32_t i, uint32_t j, uint32_t elevation, Tile_Type type) {
	set_tile(room, i, j, elevation, type, Ramp_Orientation::FLAT);
}

void print_room(Room* room) {
	for (int i = 0; i < room->depth; i++) {
		for (int j = 0; j < room->width; j++) {
			printf("Column:%d,%d: ", i, j);
			for (int k = room->height - 1; k >= 0; k--) {
				printf("%d-", TILE_AT(room, i, j, k).type);
			}
			printf("\n");
		}
	}
}

Room room_alloc(uint32_t d, uint32_t w, uint32_t h) {
	Room r = { 0 };
	r.depth = d;
	r.width = w;
	r.height = h;
	
	// tiles
	r.tiles = (Floor_Tile*)malloc(sizeof(Floor_Tile) * d * w * h);
	assert(r.tiles != NULL);
	memset(r.tiles, 0, sizeof(Floor_Tile) * d * w * h);

	return r;
}

void room_free(Room* room) {
	free(room->tiles);
	room->tiles = NULL;

	if (room->tile_boxes.data != NULL) {
		array_free(&room->tile_boxes);
	}
}

Room generate_room_example(uint32_t depth, uint32_t width, uint32_t height) {

	Room room = room_alloc(depth, width, height);

	
	for (int i = 0; i < room.depth; i++) {
		for (int j = 0; j < room.width; j++) {
			if (i*i + j*j > 1800)	continue; // circle

			// lava hole
			if ((i > 20) && (j > 5) && (i < 25) && (j < 13)) {
				set_tile(&room, i, j, 0, Tile_Type::LAVA);
				continue;
			}

			if ((j > 23) && (i > 5) && (j < 29) && (i < 13)) {
				set_tile(&room, i, j, 5, Tile_Type::EARTH);
				// add a lava tile below
				set_tile(&room, i, j, 0, Tile_Type::LAVA);
				continue;
			}

			if ((i >= 1) && (i <= 6) && (j == 25)) {
				set_tile(&room, i, j, i, Tile_Type::EARTH, Ramp_Orientation::EAST);
				// add a lava tile below
				set_tile(&room, i, j, 0, Tile_Type::LAVA);
				continue;
			}

			if ((i > 3 && i < 19) && (j > 3 && j < 19)) {
				if (j == 10)	set_tile(&room, i, j, 3, Tile_Type::STONE);
				set_tile(&room, i, j, 0, Tile_Type::WATER);// add a water tile below
			} else {
				if (j == 9 || j == 10 || j == 11) {
					set_tile(&room, i, j, 1, Tile_Type::SAND);
				} else if (j == 12) {
					set_tile(&room, i, j, 1, Tile_Type::STONE, Ramp_Orientation::SOUTH);
				} else if (j == 8) {
					set_tile(&room, i, j, 1, Tile_Type::STONE, Ramp_Orientation::NORTH);
				} else {
					set_tile(&room, i, j, 0, Tile_Type::GRASS);
				}
			}
		}
	}

	set_tile(&room, 3, 10, 2, Tile_Type::STONE, Ramp_Orientation::EAST);
	set_tile(&room, 4, 10, 3, Tile_Type::STONE, Ramp_Orientation::EAST);
	set_tile(&room, 19, 10, 2, Tile_Type::STONE, Ramp_Orientation::WEST);
	set_tile(&room, 18, 10, 3, Tile_Type::STONE, Ramp_Orientation::WEST);


	set_tile(&room, 1, 1, 1, Tile_Type::STONE, Ramp_Orientation::FLAT);

	//for (int i = 10 - 1; i >= 0; i--)
	//	set_tile(&room, 0, 10, i, Tile_Type::STONE, Ramp_Orientation::FLAT);

	return room;
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

// models only used by editor
Model_Info_For_Shading box_line_model =	    { 0 };
Model_Info_For_Shading marker_round_model = { 0 };

void init_models_for_drawing() {

	if (!init_texture_catalog_disk_and_gpu()) {
		printf("Error: failed to load some textures..\n");
		exit(1);
	}

	if (!init_model_catalog_disk()) {
		printf("Error: failed to load some models..\n");
		exit(1);
	}

	// models editor
	marker_round_model.model = g_model_catalog.names.marker_icosphere;
	marker_round_model.texture_color = &g_texture_catalog.names.plain_red;
	shader_init_model(&shader_brdf, &marker_round_model);

	box_line_model.model = construct_box_lines();
	shader_init_model(&shader_editor_box, &box_line_model);

	// player
	float scale = 0.8f; 
	player_model_info.model = load_anim_bada_file("../resources/3d_models/earth_bender_anim.bada");
	player_model_info.model.scale = scale;
	player_model_info.model.bounding_box.max = player_model_info.model.bounding_box.max * scale;
	player_model_info.model.bounding_box.min = player_model_info.model.bounding_box.min * scale;

	player_model_info.texture_color = &g_texture_catalog.names.earth_bender_color;
	shader_init_animated_model(&shader_brdf, &player_model_info);

	// models tiles
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
}

void draw_stone(Bender *p) {
	Mat4 translation = matrix_translation(Vec3{ 5, 5, 0.15f });

	shader_uniform_set(shader_brdf.gl_id, "model", translation * matrix_rotation_euler(0.0f, 0.0f, get_time()));
	shader_draw_call(&loaded_models.stone);
}

Model_Info_For_Shading* model_info_from_type(Tile_Type type, Ramp_Orientation ramp_direction) {
	
	auto model = &loaded_models.stone_tile;
	
	assert(model->initialized);

	if (ramp_direction != Ramp_Orientation::FLAT) model = &loaded_models.stone_tile_ramp;
	return model;
}

void draw_tile(Tile_Type type, Ramp_Orientation ramp_direction, float elevation, int16_t x, int16_t y) {

	if (type == Tile_Type::AIR) return;

	auto model = model_info_from_type(type, ramp_direction);

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
		break;
	case Ramp_Orientation::EAST:
		shader_uniform_set(current_shader, "model", model_matrix * model_rotation_90());
		break;
	case Ramp_Orientation::NORTH:
		shader_uniform_set(current_shader, "model", model_matrix * model_rotation_180());
		break;
	case Ramp_Orientation::WEST:
		shader_uniform_set(current_shader, "model", model_matrix * model_rotation_270());
		break;
	default:
		shader_draw_call(model);
		break;
	}

	shader_draw_call(model);

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
	shader_uniform_set(shader_editor_highlight.gl_id, "view", view);


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
	shader_uniform_set(shader_editor_highlight.gl_id, "projection", projection);


	// globals
	shader_uniform_set(shader_water.gl_id, "time", get_time());

}

void draw_room(Room* room) {

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

	Vec3 draw_pos = p->pos;
	Mat4 rotation = matrix_rotation_euler(0.0f, 0.0f, (-p->direction_angle + M_PI));
	Mat4 scale = matrix_scale(player_model_info.model.scale);
	Mat4 translation = matrix_translation(draw_pos);

	// Model
	Mat4 model = translation * rotation * scale;
	shader_uniform_set(shader_brdf.gl_id, "model", model);
	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.15f);

	const float animation_frame_time = 0.025f;
	int frame_index = (int32_t)(get_time() / animation_frame_time) % player_model_info.model.count;

	if (p->current_action == Action::IDLE) {
		frame_index = 10;
	}

	shader_draw_call(&player_model_info, frame_index);
	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.05f);

	// Debug stuff
	draw_debug_sphere(draw_pos);

	Vec3 max = player_model_info.model.bounding_box.max;
	Vec3 min = player_model_info.model.bounding_box.min;

	/*
	draw_debug_sphere(rotation * max + draw_pos);
	draw_debug_sphere(rotation * Vec3{ max.x, max.y, min.z } + draw_pos);
	draw_debug_sphere(rotation * Vec3{ max.x, min.y, max.z } + draw_pos);
	draw_debug_sphere(rotation * Vec3{ min.x, max.y, max.z } + draw_pos);
	draw_debug_sphere(rotation * min + draw_pos);
	draw_debug_sphere(rotation * Vec3{ min.x, min.y, max.z } + draw_pos);
	draw_debug_sphere(rotation * Vec3{ min.x, max.y, min.z } + draw_pos);
	draw_debug_sphere(rotation * Vec3{ max.x, min.y, min.z } + draw_pos);
	*/

	Vec3 volume = max - min;
	Mat4 box_scale = matrix_scale(volume);
	Mat4 box_trans = matrix_translation((max + min) * 0.5f);

	Mat4 transformation = translation * rotation * box_trans * box_scale ;

	shader_uniform_set(shader_editor_box.gl_id, "highlight_color", Vec4{ 0.80f, 0.10f, 0.91f, 0.9f });
	shader_uniform_set(shader_editor_box.gl_id, "model", transformation);
	shader_draw_call(&box_line_model);
}

void draw_game(Bender* bender, Room* room) {

	update_gpu_for_shading(bender);

	draw_player(bender);
	draw_room(room);
	draw_stone(bender);
}

int top_non_air_elevation(Room* room, uint32_t i, uint32_t j) {
	
	Tile_Type top_type;
	int k = room->height - 1;
	for (; k >= 0; k--) {
		top_type = TILE_AT(room, i, j, k).type;
		if (top_type != Tile_Type::AIR) {
			return k;
		}
	}
	return -1;
}

void update_player(Bender* b, Room* room) {

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
		int current_elevation = -1;

		int desired_i = (int)floorf(desired_pos.x + 0.5f);
		int desired_j = (int)floorf(desired_pos.y + 0.5f);

		Floor_Tile* desired_tile = NULL;
		Floor_Tile* current_tile = NULL;
		{
			if ((desired_i >= 0 && desired_i < room->width) && (desired_j >= 0 && desired_j < room->depth)) {
				int elevation = top_non_air_elevation(room, desired_i, desired_j);
				if (elevation != -1)
					desired_tile = &TILE_AT(room, desired_i, desired_j, elevation);
			}

			if ((current_i >= 0 && current_i < room->width) && (current_j >= 0 && current_j < room->depth)) {
				current_elevation = top_non_air_elevation(room, current_i, current_j);
				current_tile = &TILE_AT(room, current_i, current_j, current_elevation);
			}
		}

		assert(current_elevation != -1);

		// snap the x or y cordinate to previous tiles
		if (current_tile != NULL && (desired_tile == NULL) || !desired_tile->allow_walking) {
			int d_i = desired_i - current_i;
			int d_j = desired_j - current_j;

			if (d_i != 0) {
				desired_pos.x = b->pos.x;
			}

			if (d_j != 0) {
				desired_pos.y = b->pos.y;
			}

			desired_pos.z = current_elevation * 0.5f + 0.5f;
		}
		
		if (desired_tile != NULL) {

			desired_pos.z = current_elevation * 0.5f + 0.5f;

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

	uint32_t n_width = room->width;
	uint32_t n_depth = room->depth;

	for (int i = 0; i < n_width; i++) {
		for (int j = 0; j < n_depth; j++) {

			pos = { x0 + offset + (width + pad_x) * i + 0.5f * width, y0 + offset + (height + pad_y) * j + 0.5f * height };

			//immediate_quad(pos, size, color_from_tile_type(floor->tiles[i][j].type));

			Tile_Type top_type = Tile_Type::AIR;
			for (int k = room->height - 1; k >= 0; k--) {
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

	immediate_quad(offset, offset, (width + pad_x) * n_width - pad_x + 2 * x0, (height + pad_y) * n_depth - pad_y + 2 * y0, Vec4{ 0, 0, 0, 1 });
}