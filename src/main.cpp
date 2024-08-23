#include "input_and_window.hpp"
#include "rendering_backend.hpp"
#include "game.hpp"

#include <time.h>
#include "shaders.hpp"
#include "ray.hpp"

int screen_width;
int screen_height;

struct Tile_Hover {
	bool is_placable;
	Vec3 hit_position;
	Vec3 primary_pos;
	Vec3 secondary_pos;
};

struct Editor_State {
	bool initialized;

	float zoom_level;
	Vec3 camera_pos;
	Vec3 camera_direction;
	Vec3 camera_up;
	float fov;
	float near_plane;
	float far_plane;
	float aspect;

	int placeable_hovered;
	int placeable_selected;
	size_t placeable_count;
	Box *placeable_boxes;
	static const int placeables_per_row = 20;

	Room* active_room;
	Tile_Hover tile_hover;
};

Editor_State editor = { 0 };

void init_editor(Room* room) {

	editor.zoom_level = 5;

	editor.camera_pos = Vec3{ -6, -6, 15 };
	editor.camera_direction = Vec3{ 1, 1, -1.5f };
	normalize_or_z_axis(&editor.camera_direction);

	// calculate the real up direction based on forward (camera_direction)
	editor.camera_up = Vec3{ 0, 0, 1 };
	Vec3 side = cross(editor.camera_direction, editor.camera_up);
	normalize_or_z_axis(&side);
	editor.camera_up = cross(side, editor.camera_direction);
	normalize_or_z_axis(&editor.camera_up);

	// Perspective projection
	editor.fov = 60.0f;
	editor.near_plane = 0.01f;
	editor.far_plane = 1000.0f;
	editor.aspect = 1.4f;

	editor.placeable_hovered = -1;
	editor.placeable_selected = -1;
	editor.placeable_count = sizeof(loaded_models.as_array) / sizeof(loaded_models.as_array[0]);

	// TODO: handle free
	Box* box = (Box*)malloc(editor.placeable_count * sizeof(Box));
	assert(box != NULL);

	for (int i = 0; i < editor.placeable_count; i++) {
		int items_per_row = editor.placeables_per_row;
		Vec3 item_pos = Vec3{ -2 - (float)(i / items_per_row), (float)(i % items_per_row), 0 };
		//box[i].max = item_pos + Vec3{ 0.5f, 0.5f, 1.0f };
		//box[i].min = item_pos - Vec3{ -0.5f, -0.5f, 0.0f };
		box[i].max = loaded_models.as_array[i].model.bounding_box.max + item_pos;
		box[i].min = loaded_models.as_array[i].model.bounding_box.min + item_pos;
	}

	editor.placeable_boxes = box;
	editor.active_room = room;
}

void draw_debug_sphere(Vec3 pos) {
	// the icosphere
	Mat4 model = matrix_translation(pos);
	shader_uniform_set(shader_brdf.gl_id, "model", model);
	shader_draw_call(&marker_round_model);
}

void draw_debug_box(Box box, Vec4 color) {
	Vec3 max = box.max;
	Vec3 min = box.min;

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

	Vec3 dimensions = max - min;
	Mat4 box_scale = matrix_scale(dimensions);
	Mat4 box_trans = matrix_translation((max + min) * 0.5f);

	Mat4 transformation = box_trans * box_scale;

	shader_uniform_set(shader_editor_box.gl_id, "highlight_color", color);
	shader_uniform_set(shader_editor_box.gl_id, "model", transformation);
	shader_draw_call(&box_line_model);
}

void draw_hover_tile_box(Vec3 pos, Box box) {
	Vec3 max = box.max;
	Vec3 min = box.min;

	Vec3 volume = max - min;
	Mat4 box_scale = matrix_scale(volume);
	Mat4 box_trans = matrix_translation((max + min) * 0.5f);
	Mat4 translation = matrix_translation(pos);

	Mat4 transformation = translation * box_trans * box_scale;

	shader_uniform_set(shader_editor_box.gl_id, "highlight_color", Vec4{ 0.40f, 0.10f, 0.91f, 0.9f });
	shader_uniform_set(shader_editor_box.gl_id, "model", transformation);
	shader_draw_call(&box_line_model);

	draw_debug_sphere(max + pos);
	draw_debug_sphere(Vec3{ max.x, max.y, min.z } + pos);
	draw_debug_sphere(Vec3{ max.x, min.y, max.z } + pos);
	draw_debug_sphere(Vec3{ min.x, max.y, max.z } + pos);
	draw_debug_sphere(min + pos);
	draw_debug_sphere(Vec3{ min.x, min.y, max.z } + pos);
	draw_debug_sphere(Vec3{ min.x, max.y, min.z } + pos);
	draw_debug_sphere(Vec3{ max.x, min.y, min.z } + pos);
}

void draw_editor(Room* room) {
	if (!editor.initialized) {
		editor.initialized = true;
		init_editor(room);
	}

	Vec3 pos = editor.camera_pos + editor.camera_direction * editor.zoom_level;

	Mat4 view = matrix_camera(pos, editor.camera_direction, editor.camera_up);
	shader_uniform_set(shader_brdf.gl_id, "view", view);
	shader_uniform_set(shader_water.gl_id, "view", view);
	shader_uniform_set(shader_editor_highlight.gl_id, "view", view);
	shader_uniform_set(shader_editor_box.gl_id, "view", view);

	// TODO: don't need to update every frame
	Mat4 projection = matrix_perspective_projection(editor.fov, editor.aspect, editor.near_plane, editor.far_plane);
	shader_uniform_set(shader_brdf.gl_id, "projection", projection);
	shader_uniform_set(shader_water.gl_id, "projection", projection);
	shader_uniform_set(shader_editor_highlight.gl_id, "projection", projection);
	shader_uniform_set(shader_editor_box.gl_id, "projection", projection);

	shader_uniform_set(shader_water.gl_id, "time", get_time());
	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.5f);

	int selected = editor.placeable_selected;
	int hovered = editor.placeable_hovered;

	if (editor.tile_hover.is_placable) {
		// TODO: cleanup

		Vec3 next_pos = editor.tile_hover.secondary_pos;
		Vec3 pos = editor.tile_hover.primary_pos;
		//printf("position x, y, z: %f, %f, %f\n", pos.x, pos.y, pos.z);

		Mat4 scale = matrix_scale(Vec3{1.0f, 1.0f, 0.5f});

		Mat4 model = matrix_translation(pos) * scale;
		shader_uniform_set(shader_editor_box.gl_id, "model", model);
		shader_uniform_set(shader_editor_box.gl_id, "highlight_color", Vec4{ 0.00f, 0.99f, 0.91f, 0.9f });
		shader_draw_call(&box_line_model);

		if (tile_in_bounds(editor.active_room, next_pos)) {
			model = matrix_translation(next_pos) * scale;
			shader_uniform_set(shader_editor_box.gl_id, "model", model);
			shader_uniform_set(shader_editor_box.gl_id, "highlight_color", Vec4{ 0.90f, 0.29f, 0.91f, 0.9f });
			shader_draw_call(&box_line_model);
		}

		draw_debug_sphere(editor.tile_hover.hit_position);
	}

	for (int i = 0; i < editor.placeable_count; i++) {
		
		int items_per_row = editor.placeables_per_row;
		Mat4 model_rotation = matrix_rotation_euler(0, 0, 0);
		Vec3 item_pos = Vec3{ -2 - (float)(i / items_per_row), (float)(i % items_per_row), 0 };
		Mat4 translation = matrix_translation(item_pos);
		shader_uniform_set(shader_brdf.gl_id, "model", translation * model_rotation);
		shader_uniform_set(shader_water.gl_id, "model", translation * model_rotation);
		shader_draw_call(&loaded_models.as_array[i]);
		
		if (i == selected) {
			shader_uniform_set(shader_editor_highlight.gl_id, "highlight_color", Vec4{ 0.00f, 0.99f, 0.01f, 0.9f });
			Mat4 scale = matrix_scale(1.002f);
			shader_uniform_set(shader_editor_highlight.gl_id, "model", translation * model_rotation * scale);
			shader_draw_call(&loaded_models.as_array[i], &shader_editor_highlight);
		}

		if (i == hovered) {
			shader_uniform_set(shader_editor_highlight.gl_id, "highlight_color", Vec4{ 0.99f, 0.99f, 0.99f, 0.9f });
			Mat4 scale = matrix_scale(1.002f);
			shader_uniform_set(shader_editor_highlight.gl_id, "model", translation * model_rotation * scale);
			shader_draw_call(&loaded_models.as_array[i], &shader_editor_highlight);
		}
	}

	shader_uniform_set(shader_brdf.gl_id, "ambient_strength", 0.05f);
	draw_room(room);

	// draw a box enclosing the whole room
	Box box = Box{ Vec3{-0.5f, -0.5f, 0}, Vec3{room->depth - 0.5f, room->width - 0.5f, room->height * 0.5f} };
	Vec4 color = Vec4{0.99f, 0.12f, 0.12f, 1.0f};
	draw_debug_box(box, color);

}

// find the hovered block index that is placed on the side
// TODO: factor code that is copy pasted (i.e item position)
Ray construct_mouse_ray() {
	Ray ray = { 0 };

	ray.origin = editor.camera_pos + editor.camera_direction * editor.zoom_level; // TODO: refactor copy paste from above

	{
		Vec3 direction = editor.camera_direction; // assume normalized!

		float d = 1.0f / tan(editor.fov * 0.5f);
		float n_x = (((float)mouse_x / (float)screen_width)) - 0.5f;
		float n_y = ((((float)mouse_y / (float)screen_height)) - 0.5f) / editor.aspect;

		direction = direction * d;
		direction += editor.camera_up * n_y * 0.25f;

		Vec3 camera_right = cross(editor.camera_up, editor.camera_direction);
		direction += camera_right * n_x * 0.25f;

		normalize_or_z_axis(&direction);

		ray.direction = direction;
	}

	return ray;
}

void editor_find_item_hover_index() {

	editor.placeable_hovered = -1;

	Ray ray = construct_mouse_ray();	
	auto result = ray_cast(ray, editor.placeable_boxes, editor.placeable_count);
	if (result.did_hit) {
		editor.placeable_hovered = result.hit_index;
		assert(result.hit_index < editor.placeable_count);
	}
}


void editor_find_hover_place() {

	if (!editor.initialized) return;
	
	editor.tile_hover.is_placable = false;

	Ray ray = construct_mouse_ray();
	Room* room = editor.active_room;
	auto result = ray_cast(ray, room->tile_boxes.data, room->tile_boxes.count);
	
	if (result.did_hit) {
		editor.tile_hover.is_placable = true;
		Vec3 h = result.hit_object_position;
		Vec3 normal = result.normal;

		editor.tile_hover.primary_pos = h;

		// find closest normal vector of the box

		Vec3 directions[6] = {
			Vec3{ 0, 0, 1},
			Vec3{ 0, 1, 0},
			Vec3{ 1, 0, 0},
			Vec3{ 0, 0, -1},
			Vec3{ 0, -1, 0},
			Vec3{ -1, 0, 0},
		};

		float max_dot_product = -FLT_MAX;
		int max_index = 0;

		for (int i = 0; i < 6; i++) { 
			float dot_product = dot(directions[i], normal);
			if (dot_product > max_dot_product) {
				max_dot_product = dot_product;
				max_index = i;
			}
		}

		Vec3 snaped_offset = directions[max_index];
		snaped_offset.z *= 0.5f;

		editor.tile_hover.secondary_pos = snaped_offset + editor.tile_hover.primary_pos;
		editor.tile_hover.hit_position = result.hit_object_position;
	}
}

void handle_input_movement_editor() {

	Vec2 direction = { 0 };

	if ((get_key_flags_state(VK_UP) | get_key_flags_state((uint32_t)Key_Code::W)) & Key_State_Flags::DOWN) {
		direction += Vec2{ 1, 1 };
	}

	if ((get_key_flags_state(VK_DOWN) | get_key_flags_state((uint32_t)Key_Code::S)) & Key_State_Flags::DOWN) {
		direction += Vec2{ -1, -1 };
	}

	if ((get_key_flags_state(VK_RIGHT) | get_key_flags_state((uint32_t)Key_Code::D)) & Key_State_Flags::DOWN) {
		direction += Vec2{ 1, -1 };
	}

	if ((get_key_flags_state(VK_LEFT) | get_key_flags_state((uint32_t)Key_Code::A)) & Key_State_Flags::DOWN) {
		direction += Vec2{ -1, 1 };
	}

	normalize_or_zero(&direction);

	editor.camera_pos += Vec3{direction.x, direction.y, 0} * get_frame_time() * 15;

	editor_find_item_hover_index();
	editor_find_hover_place();

	if ((get_key_flags_state((uint32_t)Key_Code::LEFT_BTN) & Key_State_Flags::BEGIN)) {
		if (editor.placeable_hovered >= 0) {
			editor.placeable_selected = editor.placeable_hovered;
		}
		else if (editor.tile_hover.is_placable) {
			uint32_t i = (uint32_t)editor.tile_hover.secondary_pos.x;
			uint32_t j = (uint32_t)editor.tile_hover.secondary_pos.y;
			uint32_t k = (uint32_t)(editor.tile_hover.secondary_pos.z * 2.0f);
			set_tile(editor.active_room, i, j, k, Tile_Type::STONE);
		}
	}

	if ((get_key_flags_state((uint32_t)Key_Code::RIGHT_BTN) & Key_State_Flags::BEGIN)) {
		uint32_t i = (uint32_t)editor.tile_hover.primary_pos.x;
		uint32_t j = (uint32_t)editor.tile_hover.primary_pos.y;
		uint32_t k = (uint32_t)(editor.tile_hover.primary_pos.z * 2.0f);
		if (tile_in_bounds(editor.active_room, i, j, k)) {
			remove_tile(editor.active_room, i, j, k);
		}
	}
}

void handle_input_walking(Bender &bender) {

	bender.current_action = Action::IDLE;
	Vec2 direction = { 0 };

	if ((get_key_flags_state(VK_UP) | get_key_flags_state((uint32_t)Key_Code::W)) & Key_State_Flags::DOWN) {
		bender.current_action = Action::WALKING;
		direction += Vec2{ 1, 1 };
	}

	if ((get_key_flags_state(VK_DOWN) | get_key_flags_state((uint32_t)Key_Code::S)) & Key_State_Flags::DOWN) {
		bender.current_action = Action::WALKING;
		direction += Vec2{ -1, -1 };
	}

	if ((get_key_flags_state(VK_RIGHT) | get_key_flags_state((uint32_t)Key_Code::D)) & Key_State_Flags::DOWN) {
		bender.current_action = Action::WALKING;
		direction += Vec2{ 1, -1 };
	}

	if ((get_key_flags_state(VK_LEFT) | get_key_flags_state((uint32_t)Key_Code::A)) & Key_State_Flags::DOWN) {
		bender.current_action = Action::WALKING;
		direction += Vec2{ -1, 1 };
	}

	normalize_or_y_axis(&direction);

	if (bender.current_action == Action::WALKING)
		bender.target_direction_angle = angle_between(direction, Vec2{ 0, 1 });

	if (get_key_flags_state((uint32_t)Key_Code::SPACE) & Key_State_Flags::BEGIN) {
		bender.jumping = true;
		bender.jump_vel = 15.0f;
	}
}

enum class Program_State {
	GAME,
	EDITOR,
};

int main() {

	screen_width = 1280;
	screen_height = 860;

	Window_Info window_info = create_window(screen_width, screen_height);
	backend_init(&window_info);

	Window_Info_For_Restore saved_window = { 0 };

	bool quit = false;
	bool fullscreen = false;

	float shake_timer = 0.0f;

	Vec4 color = { 1.0, 0.5, 0.0, 1.0 };

	init_models_for_drawing();

	Room room = generate_room_example(40, 40, 18);

	draw_room(&room);

	Bender bender = { 0 };
	bender.pos = Vec3{1.0f, 1.0f, 0.5f };
	bender.direction_angle = 0.0f;
	bender.aspect = (float)screen_width /(float)screen_height;

	bender.turn_speed = 6.0f;
	bender.walk_speed = 4.0f;

	bender.fov = 50.0f;

	Program_State program_state = Program_State::EDITOR;

	while (!quit) {

		update_window_events();

		if (program_state == Program_State::GAME) {
			handle_input_walking(bender);
		}
		
		if (program_state == Program_State::EDITOR) {
			handle_input_movement_editor();
		}

		for (int i = 0; i < events_this_frame.count; i++) {
			Event event = events_this_frame.data[i];
			printf("event type:%d key_state:0x%x\n", event.type, event.key_state);

			// common events			
			if (event.type == QUIT) quit = true;
			if ((event.type == KEYBOARD) && (event.key_state & BEGIN)) {
				if (event.key_code == Key_Code::ESCAPE) {
					quit = true;
				}

				if (event.key_code == Key_Code::F && (event.modifiers & CTRL)) {
					fullscreen = !fullscreen;
					Vec2 new_size = toggle_fullscreen(window_info.window_handle, fullscreen, &saved_window);
					screen_width = new_size.x;
					screen_height = new_size.y;
					adjust_viewport_size(screen_width, screen_height);
					float aspect = (float)screen_width / (float)screen_height;
				}

				if (event.key_code == Key_Code::O) {
					bender.use_orthographic = !bender.use_orthographic;
				}

				if (event.key_code == Key_Code::TAB) {
					if (program_state == Program_State::GAME)
						program_state = Program_State::EDITOR;
					else 
						program_state = Program_State::GAME;
				}
			}

			// game specific events
			if (program_state == Program_State::GAME) {
				if ((event.type == KEYBOARD) && (event.key_state & BEGIN)) {
					if (event.key_code == Key_Code::LEFT_BTN) {
						color.x = (float)rand() / RAND_MAX;
						color.y = (float)rand() / RAND_MAX;
						color.z = (float)rand() / RAND_MAX;
						shake_timer = 0.5f;
					}
				}
			}

			// editor specific events
			if (program_state == Program_State::EDITOR) {
				if (event.type == MOUSE_WHEEL_V) {
					editor.zoom_level += event.wheel_delta * 0.02f;
					clamp(&editor.zoom_level, 0, 20.6f); // heuristic
				}
			}

		}
		events_reset();

		//
		// update code
		//

		if (program_state == Program_State::GAME) {
			update_player(&bender, &room);
			//printf("action %d, pos (%f %f), target_angle %f, angle %f\n", player.current_action, player.pos.x, player.pos.x, player.target_direction_angle, player.direction_angle);
		}
		
		//
		// draw code
		//

		clear_it(0.15f, 0.15f, 0.15f, 1.0f);
		
		if (program_state == Program_State::GAME) {
			draw_minimap(&room, &bender);
			draw_game(&bender, &room);

			// ui stuff
			float shake_amount = shake_timer;
			Vec3 offset = { shake_amount * 0.02f*cosf(get_time() * 102), shake_amount * 0.03f*sinf(get_time() * 109), 0 };
			Mat4 translation = matrix_translation(offset);
			Mat4 scale = matrix_scale(lerp(1.0f, 0.95f, shake_amount*shake_amount));
		
			glUseProgram(immediate_shader_color.gl_id);
			shader_uniform_set(immediate_shader_color.gl_id, "projection", translation*scale);

			if (shake_timer > 0.0f) {
				shake_timer -= get_frame_time();
				if (shake_timer < 0.0f) {
					shake_timer = 0.0f;
				}
			}

			immediate_send();
		}

		if (program_state == Program_State::EDITOR) {
			draw_editor(&room);
		}

		swap_buffers(&window_info);

		//printf("ctrl:%d shift:%d alt:%d meta:%d \n", ctrl_state, shift_state, alt_state, meta_state);
		//printf("get_time() %f frame_time %f FPS %.2f \n", get_time(), get_frame_time(), get_fps());

	}

	destroy_window(window_info.window_handle);
}

static int APIENTRY WinMain( _In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow)
{
	return main();
}