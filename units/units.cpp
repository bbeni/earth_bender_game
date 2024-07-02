#include "input_and_window.hpp"
#include "rendering_backend.hpp"
#include "game.hpp"

#include <time.h>

int main() {

    int width = 1080;
    int height = 720;

    Window_Info window_info = create_window(1080, 720);
    backend_init(&window_info);

    Window_Info_For_Restore saved_window = { 0 };

    bool quit = false;
    bool fullscreen = false;

    Floor floor = {0};
    generate_floor(&floor);

    Vec4 color = { 1.0, 0.5, 0.0, 1.0 };


    while (!quit) {
        update_window_events();

        for (int i = 0; i < events_this_frame.count; i++) {
            Event event = events_this_frame.data[i];
            printf("event type:%d key_state:0x%x\n", event.type, event.key_state);
            
            if (event.type == QUIT) quit = true;

            // Action on Press Key / Click
            if ((event.type == KEYBOARD) && (event.key_state & BEGIN)) {
                if (event.key_code == Key_Code::ESCAPE) {
                    quit = true;
                }

                if (event.key_code == Key_Code::LEFT_BTN) {
                    color.x = (float)rand() / RAND_MAX;
                    color.y = (float)rand() / RAND_MAX;
                    color.z = (float)rand() / RAND_MAX;
                }

                if (event.key_code == Key_Code::F && (event.modifiers & CTRL)) {
                    fullscreen = !fullscreen;
                    Vec2 new_size = toggle_fullscreen(window_info.window_handle, fullscreen, &saved_window);
                    width = new_size.x;
                    height = new_size.y;
                    adjust_viewport_size(width, height);
                }
            }
        }

        events_reset();


        float r = sinf(get_time() * 3.0f) * 0.5f + 0.5f;
        float g = sinf(get_time() * 4.3f) * 0.5f + 0.5f;

        clamp(&r, 0.0f, 1.0f);

        clear_it(r, 0.2f, g, 1.0f);

        draw_floor(&floor);

        Vec2 pos = { (float)2*mouse_x/width - 1.0f, -(float)2*mouse_y/height + 1.0f };
        Vec2 size = { 0.15f, 0.25f };

        immediate_quad(pos, size, color);

        Vec4 fg_color = color;
        size.x -= 0.02f;
        size.y -= 0.03f;
        fg_color.x += 0.2f;
        fg_color.y += 0.2f;
        fg_color.z += 0.2f;
      
        clamp(&fg_color.x, 0, 1);
        clamp(&fg_color.y, 0, 1);
        clamp(&fg_color.z, 0, 1);

        immediate_quad(pos, size, fg_color);
        
        immediate_send();
        swap_buffers(&window_info);

        //printf("ctrl:%d shift:%d alt:%d meta:%d \n", ctrl_state, shift_state, alt_state, meta_state);
        //printf("get_time() %f frame_time %f FPS %.2f \n", get_time(), get_frame_time(), get_fps());

    }

    destroy_window(window_info.window_handle);
}

