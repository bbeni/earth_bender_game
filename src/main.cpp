#include "input_and_window.hpp"
#include "rendering_backend.hpp"
#include "game.hpp"

#include <time.h>
#include "shaders.hpp"


Model_Info_For_Shading cube = {0};


struct Editor_State {
	bool initialized;
	Vec3 camera_pos;
	float zoom_level;
};

Editor_State editor = {0};


void draw_editor(Level *level) {
	if (!editor.initialized) {
		editor.initialized = true;
		make_cube_model(&cube.model);
		cube.texture_color = &g_texture_catalog.names.default_color;
		shader_init_model(&shader_brdf, &cube);
	}

	Mat4 model_rotation = matrix_from_basis_vectors({ 1,0,0 }, { 0,1,0 }, { 0,0,1 });
	Mat4 translation = matrix_translation(Vec3{ 0, 0, 0.5f });
	shader_uniform_set(shader_brdf.gl_id, "model", translation * model_rotation);
	shader_uniform_set(shader_water.gl_id, "model", translation * model_rotation);

	Vec3 looking_direction = Vec3{ 1, 1, -1.5f };
	Vec3 pos = Vec3{ -4, -4, 20 } + editor.camera_pos + looking_direction * editor.zoom_level;

	Mat4 view = matrix_camera(pos, looking_direction, Vec3{0, 0, 1});
	shader_uniform_set(shader_brdf.gl_id, "view", view);
	shader_uniform_set(shader_water.gl_id, "view", view);
	shader_uniform_set(shader_water.gl_id, "time", get_time());

	shader_draw_call(&cube);

	draw_floor(level);

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

	editor.camera_pos += Vec3{direction.x, direction.y, 0} * get_frame_time() * 10;
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

}

enum class Program_State {
	GAME,
	EDITOR,
};

int main() {

	int width = 1280;
	int height = 860;

	Window_Info window_info = create_window(width, height);
	backend_init(&window_info);

	Window_Info_For_Restore saved_window = { 0 };

	bool quit = false;
	bool fullscreen = false;

	float shake_timer = 0.0f;

	Level floor = {0};
	generate_floor(&floor);

	Vec4 color = { 1.0, 0.5, 0.0, 1.0 };

	init_models_for_drawing();
	draw_floor(&floor);

	Bender bender = { 0 };
	bender.pos = Vec3{1.0f, 1.0f, 0.5f };
	bender.direction_angle = 0.0f;
	bender.aspect = (float)width/(float)height;

	bender.turn_speed = 6.0f;
	bender.walk_speed = 4.0f;

	Program_State program_state = Program_State::GAME;

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
					width = new_size.x;
					height = new_size.y;
					adjust_viewport_size(width, height);
					float aspect = (float)width / (float)height;
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
					editor.zoom_level += event.wheel_delta * 0.004f;
					clamp(&editor.zoom_level, -7, 12.7f); // heuristic
				}
			}

		}
		events_reset();

		//
		// update code
		//

		update_player(&bender, &floor);
		//printf("action %d, pos (%f %f), target_angle %f, angle %f\n", player.current_action, player.pos.x, player.pos.x, player.target_direction_angle, player.direction_angle);
		
		
		//
		// draw code
		//

		clear_it(0.25f, 0.35f, 0.85f, 1.0f);


		if (program_state == Program_State::GAME) {
			draw_minimap(&floor, &bender);
			draw_game(&bender, &floor);

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

			Vec2 pos = { (float)2*mouse_x/width - 1.0f, -(float)2*mouse_y/height + 1.0f };
			Vec2 size = { 0.05f, 0.075f };

			immediate_quad(pos, size, color);

			Vec4 fg_color = color;
			size.x += 0.02f;
			size.y += 0.03f;
			fg_color.x += 0.2f;
			fg_color.y += 0.2f;
			fg_color.z += 0.2f;

			clamp(&fg_color.x, 0, 1);
			clamp(&fg_color.y, 0, 1);
			clamp(&fg_color.z, 0, 1);

			immediate_quad(pos, size, fg_color);

			immediate_send();
		}

		if (program_state == Program_State::EDITOR) {
			draw_editor(&floor);
		}

		swap_buffers(&window_info);

		//printf("ctrl:%d shift:%d alt:%d meta:%d \n", ctrl_state, shift_state, alt_state, meta_state);
		//printf("get_time() %f frame_time %f FPS %.2f \n", get_time(), get_frame_time(), get_fps());

	}

	destroy_window(window_info.window_handle);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return main();
}