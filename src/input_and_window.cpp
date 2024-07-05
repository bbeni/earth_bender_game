#include "input_and_window.hpp"

#include <cassert>
#include <stdio.h>
#include <time.h>

// initialization of the globals to use in api

Events_Array events_this_frame;

int mouse_x;
int mouse_y;
int mouse_delta_x;
int mouse_delta_y;
Key_State_Flags mouse_state;

bool shift_state = false;
bool ctrl_state = false;
bool alt_state = false;
bool meta_state = false;

std::map<uint32_t, bool>            key_down_table;
std::map<uint32_t, Key_State_Flags> key_flags_table;

// Winodw stuff

bool have_window_class = FALSE;

Window_Info create_window(int width, int height) {

    const wchar_t* window_class_name = L"WhatAWindowClass";

    if (!have_window_class) {
        HMODULE hInstance = GetModuleHandleW(NULL);
        assert(hInstance != 0);

        HICON icon = LoadIconW(hInstance, (LPCWSTR)2);

        if (icon == NULL) {

            // TODO make this work
            //const wchar_t exe_path[MAX_PATH] = {0};
            //GetModuleFileNameW(NULL, (LPWSTR)exe_path, MAX_PATH);
            //icon = ExtractIconW(hInstance, exe_path, 0);

            icon = LoadIconW(NULL, IDI_APPLICATION);

            WNDCLASSEXW c;
            c.cbSize = sizeof(WNDCLASSEXW);
            c.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            c.lpfnWndProc = my_window_proc;
            c.cbClsExtra = 0;
            c.cbWndExtra = 0;
            c.hIcon = icon;
            c.hCursor = LoadCursorW(NULL, IDC_ARROW);
            c.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
            c.lpszMenuName = NULL;
            c.lpszClassName = L"WhatAWindowClass";
            c.hInstance = GetModuleHandleW(NULL);
            c.hIconSm = LoadIconW(NULL, IDI_APPLICATION);

            if (RegisterClassExW(&c) == 0) {
                printf("Error: RegisterClassExW failed with error code %lu\n", GetLastError());
            }

            have_window_class = TRUE;
        }
    }

    int x = 0;
    int y = 0;

    HWND hwnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        L"WhatAWindowClass",
        L"Fighting with the windows",
        WS_OVERLAPPEDWINDOW,
        x, y,
        width, height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (hwnd == NULL) {
        printf("Error: CreateWindowExW failed with return value 0.\n");
        Window_Info window_info = { 0 };
        return window_info;
    }

    UpdateWindow(hwnd);
    ShowWindow(hwnd, SW_SHOW);

    Window_Info window_info = { 0 };
    window_info.window_handle = hwnd;
    return window_info;
}

void destroy_window(HWND window_handle) {
    PostQuitMessage(0);
    DestroyWindow(window_handle);
}

// returns the new size of the window
Vec2 toggle_fullscreen(HWND hwnd, bool want_fullscreen, Window_Info_For_Restore* info) {
    assert(info != 0);

    int32_t width;
    int32_t height;

    if (want_fullscreen) {
        uint32_t old_style = GetWindowLongW(hwnd, GWL_STYLE);
        uint32_t old_ex_style = GetWindowLongW(hwnd, GWL_EXSTYLE);

        SetWindowLongW(hwnd, GWL_STYLE, old_style & ~(uint32_t)(WS_CAPTION | WS_THICKFRAME));
        SetWindowLongW(hwnd, GWL_EXSTYLE, old_ex_style & ~(uint32_t)(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

        HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mon_info;
        mon_info.cbSize = sizeof(MONITORINFO);
        bool success = GetMonitorInfoW(monitor, &mon_info);
        if (success) {
            GetWindowRect(hwnd, &(info->windows_rectangle));
            info->style = old_style;
            info->extended_style = old_ex_style;

            int32_t x = mon_info.rcMonitor.left;
            int32_t y = mon_info.rcMonitor.top;
            width = mon_info.rcMonitor.right - x;
            height = mon_info.rcMonitor.bottom - y;
            SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        } else {
            int32_t x = info->windows_rectangle.left;
            int32_t y = info->windows_rectangle.top;
            width = info->windows_rectangle.right - x;
            height = info->windows_rectangle.bottom - y;
            printf("Error: failed to set fullscren!\n");
        }
    } else {
        int32_t x = info->windows_rectangle.left;
        int32_t y = info->windows_rectangle.top;
        width = info->windows_rectangle.right - x;
        height = info->windows_rectangle.bottom - y;

        SetWindowLongW(hwnd, GWL_STYLE, info->style);
        SetWindowLongW(hwnd, GWL_EXSTYLE, info->extended_style);

        SetWindowPos(hwnd, HWND_TOP, x, y, width, height, SWP_FRAMECHANGED);
    }

    Vec2 window_size = { (float)width, (float)height };
    return window_size;
}



// Input stuff

inline bool is_printable(char32_t code) {
    return (code >= 0x20) && (code != 0x7F);
}

// returns the previous down state
bool set_key_down_state(uint32_t vkey, bool is_down, Key_State_Flags* new_flags) {

    bool previous_state = false;

    if (key_down_table.count(vkey) > 0) {
        previous_state = key_down_table[vkey];
    }

    *new_flags = (Key_State_Flags)is_down;

    if (is_down && !previous_state) {
        *new_flags = (Key_State_Flags)(*new_flags | Key_State_Flags::BEGIN);
    }
    else if (!is_down && previous_state) {
        *new_flags = (Key_State_Flags)(*new_flags | Key_State_Flags::END);
    }

    key_down_table.insert_or_assign(vkey, is_down);
    key_flags_table.insert_or_assign(vkey, *new_flags);

    return is_down;
}

Key_State_Flags get_key_flags_state(uint32_t vkey) {
    if (key_down_table.count(vkey) > 0) {
        return key_flags_table[vkey];
    }
    return (Key_State_Flags)0;
}

void send_vk_event(uint64_t vkey, bool key_down, bool repeat, bool extended) {

    // TODO handle reapeat

    Key_State_Flags new_flags;
    bool previous_down = set_key_down_state((uint32_t)vkey, key_down, &new_flags);

    Key_Code key_code = (Key_Code)vkey;//get_key_code(vkey);

    if (key_code == Key_Code::ALT) alt_state = key_down;
    if (key_code == Key_Code::SHIFT) shift_state = key_down;
    if (key_code == Key_Code::CONTROL) ctrl_state = key_down;
    if (key_code == Key_Code::META_L) meta_state = key_down;

    Event event;

    event.key_code = key_code;
    event.utf_32 = (char32_t)key_code;
    event.printable = is_printable((char32_t)key_code);
    event.key_pressed = key_down;
    event.modifiers = (Modifier_Flags)((alt_state << 0) | (ctrl_state << 1) | (shift_state << 2) | (meta_state << 3));
    event.key_state = new_flags;
    event.type = KEYBOARD;

    event_add(event);
}

uint32_t wm_char_high_surrogate;

// Custom window procedure
LRESULT CALLBACK my_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_MOUSEMOVE:
    {
        // NO Mouse Move events, because there are maybe too many!
        // just 4 globals get updated
        //GET_X_LPARAM(lParam);
        int x = (int)(lParam) & 0xFFFF;
        int y = ((int)(lParam) >> 16) & 0xFFFF;
        mouse_delta_x = x - mouse_x;
        mouse_delta_y = y - mouse_y;
        mouse_x = x;
        mouse_y = y;
        break;
    }
    case WM_DESTROY:
    case WM_QUIT:
    case WM_CLOSE:
    {
        Event event;
        event.type = Event_Type::QUIT;
        event_add(event);
        break;
    }
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {

        // first 2 bytes is reapeat count
        // 1 byte scancode OEM dependent
        // bit 24 -> extended
        // ...

        bool extended = (int32_t)lParam & (0x1 << 24);
        bool repeat = (((int32_t)lParam & 0x40000000) != 0);
        send_vk_event(wParam, true, repeat, extended);
        break;
    }
    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        bool extended = (int32_t)lParam & (0x1 << 24);
        bool repeat = (((int32_t)lParam & 0x40000000) != 0);
        send_vk_event(wParam, false, repeat, extended);
        break;
    }
    case WM_CHAR:

        if (wParam >= HIGH_SURROGATE_START && wParam <= HIGH_SURROGATE_END) {
            wm_char_high_surrogate = (uint32_t)wParam;
        }
        else {
            uint32_t codepoint = (uint32_t)wParam;

            if (codepoint >= LOW_SURROGATE_START && codepoint <= LOW_SURROGATE_END) {
                uint32_t low_surrogate = (uint32_t)wParam;
                codepoint = (wm_char_high_surrogate - HIGH_SURROGATE_START) << 10;
                codepoint += (low_surrogate - LOW_SURROGATE_START);
                codepoint += 0x10000;
            }

            if (codepoint > 31 && codepoint != 127) {
                Event event;
                event.type = TEXT_INPUT;
                event.utf_32 = codepoint;
                event.key_code = (Key_Code)codepoint;
                event.printable = true;

                event_add(event);
            }
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        send_vk_event(VK_LBUTTON, uMsg == WM_LBUTTONDOWN, false, false);
        if (uMsg == WM_LBUTTONDOWN) SetCapture(hwnd); else ReleaseCapture();
        break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        send_vk_event(VK_RBUTTON, uMsg == WM_RBUTTONDOWN, false, false);
        if (uMsg == WM_RBUTTONDOWN) SetCapture(hwnd); else ReleaseCapture();
        break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        send_vk_event(VK_MBUTTON, uMsg == WM_MBUTTONDOWN, false, false);
        if (uMsg == WM_MBUTTONDOWN) SetCapture(hwnd); else ReleaseCapture();
        break;
    case WM_MOUSEWHEEL:
    {
        Event event;
        event.type = MOUSE_WHEEL_V;
        event.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
        event_add(event);
        break;
    }
    case WM_MOUSEHWHEEL:
    {
        Event event;
        event.type = MOUSE_WHEEL_H;
        event.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
        event_add(event);
        break;
    }
    case WM_PAINT:
    {
        //PAINTSTRUCT ps;
        //HDC hdc = BeginPaint(hwnd, &ps);

        //HBRUSH hBrush = CreateSolidBrush(RGB(0, 128, 128));
        //FillRect(hdc, &ps.rcPaint, hBrush);

        //DeleteObject(hBrush);
        //EndPaint(hwnd, &ps);
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        break;
    }
    case WM_SYSCHAR:
        return 0;

    case WM_ACTIVATEAPP:
        // TODO: handle activating the app
        // decide what to do to have reliable keydown states
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);


        // Not yet handled...
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    default:
        printf("Unhandled: 0x%x\n", uMsg);
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


// Event Stuff


void init_input_system() {

    return; // disable for now;

    RAWINPUTDEVICE rid[2];

    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x02;            // HID mouse
    rid[0].dwFlags = 0;
    // rid[0].hwndTarget = null;

    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x06;            // HID keyboard
    rid[1].dwFlags = 0;               //RIDEV_NOHOTKEYS;
    rid[1].hwndTarget = NULL;

    if (RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)) != 1) {
        printf("Error: Failed to register raw input. \n");
        return;
    }
}




// for handling get_frame_time()
double time_was_last_frame = 0;
double frame_time = 0;

bool initted = false;

void update_window_events() {

    double now = get_time();
    frame_time = now - time_was_last_frame;
    time_was_last_frame = now;

    if (!initted) {
        init_input_system();
        initted = true;
    }

    //TODO make sure we are in sync 
    //if (GetAsyncKeyState(VK_MENU))
    //GetAsyncKeyState(VK_CONTROL);
    //GetAsyncKeyState(VK_SHIFT)
    //GetAsyncKeyState(VK_MENU);

    while (true) {
        MSG msg;

        bool result = PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);
        if (!result)
            break;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void event_add(Event event) {
    if (events_this_frame.count < MAX_EVENTS - 1) {
        events_this_frame.data[events_this_frame.count++] = event;
    }
    else {
        printf("Error: Unhandeled events_this_frame overflown MAX_EVENTS, skipping.\n");
    }
}

void events_reset() {
    events_this_frame.count = 0;
}


// time stuff

unsigned long long int nanos_since_started = 0; // assuming it's never 0

double get_time() {
    timespec ts = { 0 };
    (void)timespec_get(&ts, TIME_UTC);
    unsigned long long int nano_secs = (unsigned long long int)ts.tv_sec * 1000000000LLU + (unsigned long long int)ts.tv_nsec;

    if (nanos_since_started == 0) {
        nanos_since_started = nano_secs;
        return 0.0;
    }

    double time = (double)(nano_secs - nanos_since_started) * 1e-9;
    return time;
}

double get_frame_time() {
    return frame_time;
}

float get_fps() {
    return 1.0f / frame_time;
}

