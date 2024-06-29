#include "input_and_window.hpp"


int main() {

    HWND hwnd = create_window(1080, 720);

    bool quit = false;
    bool fullscreen = false;

    Window_Info_For_Restore saved_window;

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
                    quit = true;
                }

                if (event.key_code == Key_Code::F && (event.modifiers & CTRL)) {
                    fullscreen = !fullscreen;
                    toggle_fullscreen(hwnd, fullscreen, &saved_window);
                }
            }

        }

        events_reset();

        //printf("ctrl:%d shift:%d alt:%d meta:%d \n", ctrl_state, shift_state, alt_state, meta_state);

        Sleep(10);
    }

    destroy_window(hwnd);
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
