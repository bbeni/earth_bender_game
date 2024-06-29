/*
alt, win, ctrl, shift  -> states
mouse_x, mouse_y       -> states
key_down_states        -> bool table
key_flags              -> flags table
mouse_flags            -> flags
*/
#ifndef INPUT_AND_WINDOW_H
#define INPUT_AND_WINDOW_H

#include "Windows.h"

#include <cstdint>
#include <map>
#include <stdbool.h>


enum Event_Type : uint32_t {
    UNINITIALIZED,
    KEYBOARD, // also mouse btn events
    TEXT_INPUT,
    WINDOW,
    MOUSE_WHEEL_V,
    MOUSE_WHEEL_H,
    QUIT,
    DRAG_AND_DROP_FILES,
};

enum Key_State_Flags : uint32_t {
    NONE  = 0x0,
    DOWN  = 0x1,
    BEGIN = 0x2,
    END   = 0x4,
};

enum Modifier_Flags : uint32_t {
    ALT   = 0x1,
    CTRL  = 0x2,
    SHIFT = 0x4,
    META  = 0x8,
};

enum class Key_Code : char32_t;

typedef struct Event {
    Event_Type		type;
	Key_Code        key_code;
	bool			key_pressed;

	Key_State_Flags key_state;
    Modifier_Flags  modifiers;
	uint32_t        utf_32; // utf32 codepoint
	bool			printable;

	float			wheel_delta; // MOUSE_WHEEL_V, MOUSE_WHEEL_H events
} Event;

#define MAX_EVENTS 1000
typedef struct Events_Array {
	Event data[MAX_EVENTS];
	int count = 0;
} Events_Array;

// globals to interact with the input...

extern Events_Array events_this_frame;

extern Key_State_Flags mouse_state;
extern int mouse_x;
extern int mouse_y;
extern int mouse_delta_x;
extern int mouse_delta_y;

extern bool shift_state;
extern bool ctrl_state;
extern bool alt_state;
extern bool meta_state;

extern bool initted;

extern std::map<uint32_t, bool>            key_down_table;
extern std::map<uint32_t, Key_State_Flags> key_flags_table;

// functions to handle events...

void events_reset();
void update_window_events();
void event_add(Event event);

// window functions

HWND create_window(int width, int height);
void destroy_window(HWND hwnd);
typedef struct Window_Info_For_Restore;
void toggle_fullscreen(HWND hwnd, bool want_fullscreen, Window_Info_For_Restore* info);

// internal

typedef struct Window_Info_For_Restore {
	RECT windows_rectangle;
	int32_t style;
	int32_t extended_style;
} Window_Info_For_Restore;

LRESULT CALLBACK my_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

enum class Key_Code : char32_t {
	UNKNOWN		= 0x00,
	LEFT_BTN    = 0x01, 	//Left mouse button
	RIGHT_BTN   = 0x02, 	//Right mouse button
	CANCEL 	    = 0x03, 	//Control - break processing
	MIDDLE_BTN 	= 0x04, 	//Middle mouse button
	FORWARD_BTN = 0x05, 	//X1 mouse button
	BACKWARD_BTN = 0x06, 	//X2 mouse button
	BACK 		= 0x08, 	//BACKSPACE key
	TAB 	    = 0x09, 	//TAB key
	CLEAR 		= 0x0C, 	//CLEAR key
	RETURN 		= 0x0D, 	//ENTER key
	SHIFT 		= 0x10, 	//SHIFT key
	CONTROL 	= 0x11, 	//CTRL key
	ALT 		= 0x12, 	//ALT key
	PAUSE 		= 0x13, 	//PAUSE key
	CAPS_LOCK 	= 0x14, 	//CAPS LOCK key
	//VK_KANA 	= 0x15 	//IME Kana mode
	//VK_HANGUL 	= 0x15 	//IME Hangul mode
	//VK_IME_ON 	= 0x16 	//IME On
	//VK_JUNJA 	= 0x17 	//IME Junja mode
	//VK_FINAL 	= 0x18 	//IME final mode
	//VK_HANJA 	= 0x19 	//IME Hanja mode
	//VK_KANJI 	= 0x19 	//IME Kanji mode
	//VK_IME_OFF 	= 0x1A 	//IME Off
	ESCAPE 		= 0x1B, 	//ESC key
	//VK_CONVERT 	= 0x1C 	//IME convert
	//VK_NONCONVERT 	= 0x1D 	//IME nonconvert
	//VK_ACCEPT 	= 0x1E 	//IME accept
	//VK_MODECHANGE 	= 0x1F 	//IME mode change request
	SPACE 	    = 0x20, 	//SPACEBAR
	PAGE_UP 	= 0x21, 	//PAGE UP key
	PAGE_DOWN 	= 0x22, 	//PAGE DOWN key
	END 		= 0x23, 	//END key
	HOME 		= 0x24, 	//HOME key
	LEFT 		= 0x25, 	//LEFT ARROW key
	UP 			= 0x26, 	//UP ARROW key
	RIGHT 		= 0x27, 	//RIGHT ARROW key
	DOWN 		= 0x28, 	//DOWN ARROW key
	SELECT 		= 0x29, 	//SELECT key
	PRINT 		= 0x2A, 	//PRINT key
	//EXECUTE 	= 0x2B 	EXECUTE key
	SNAPSHOT	= 0x2C, 	//PRINT SCREEN key
	INSERT 		= 0x2D, 	//INS key
	DEL 		= 0x2E, 	//DEL key
	//VK_HELP 	= 0x2F 	HELP key
	K0			= 0x30, 	//0 key
	K1			= 0x31, 	//1 key
	K2			= 0x32, 	//2 key
	K3			= 0x33, 	//3 key
	K4			= 0x34, 	//4 key
	K5			= 0x35, 	//5 key
	K6			= 0x36, 	//6 key
	K7			= 0x37, 	//7 key
	K8			= 0x38, 	//8 key
	K9			= 0x39, 	//9 key
	//- = 0x3A - 40 	Undefined
	A			= 0x41, 	//A key
	B			= 0x42, 	//B key
	C			= 0x43, 	//C key
	D			= 0x44, 	//D key
	E			= 0x45, 	//E key
	F			= 0x46, 	//F key
	G			= 0x47, 	//G key
	H			= 0x48, 	//H key
	I			= 0x49, 	//I key
	J			= 0x4A, 	//J key
	K			= 0x4B, 	//K key
	L			= 0x4C, 	//L key
	M			= 0x4D, 	//M key
	N			= 0x4E, 	//N key
	O			= 0x4F, 	//O key
	P			= 0x50, 	//P key
	Q			= 0x51, 	//Q key
	R			= 0x52, 	//R key
	S			= 0x53, 	//S key
	T			= 0x54, 	//T key
	U			= 0x55, 	//U key
	V			= 0x56, 	//V key
	W			= 0x57, 	//W key
	X			= 0x58, 	//X key
	Y			= 0x59, 	//Y key
	Z			= 0x5A, 	//Z key
	META_L 		= 0x5B, 	//Left Windows key
	META_R 		= 0x5C, 	//Right Windows key
	APPS 		= 0x5D, 	//Applications key
	//VK_SLEEP 	= 0x5F, 	Computer Sleep key
	NUMPAD0 	= 0x60, 	//Numeric keypad 0 key
	NUMPAD1 	= 0x61, 	//Numeric keypad 1 key
	NUMPAD2 	= 0x62, 	//Numeric keypad 2 key
	NUMPAD3 	= 0x63, 	//Numeric keypad 3 key
	NUMPAD4 	= 0x64, 	//Numeric keypad 4 key
	NUMPAD5 	= 0x65, 	//Numeric keypad 5 key
	NUMPAD6 	= 0x66, 	//Numeric keypad 6 key
	NUMPAD7 	= 0x67, 	//Numeric keypad 7 key
	NUMPAD8 	= 0x68, 	//Numeric keypad 8 key
	NUMPAD9 	= 0x69, 	//Numeric keypad 9 key
	MULTIPLY 	= 0x6A, 	//Multiply key
	ADD 	    = 0x6B, 	//Add key
	SEPARATOR 	= 0x6C, 	//Separator key
	SUBTRACT 	= 0x6D, 	//Subtract key
	DECIMAL 	= 0x6E, 	//Decimal key
	DIVIDE 	    = 0x6F, 	//Divide key
	F1 			= 0x70, 	//F1 key
	F2 			= 0x71, 	//F2 key
	F3 			= 0x72, 	//F3 key
	F4 			= 0x73, 	//F4 key
	F5 			= 0x74, 	//F5 key
	F6 			= 0x75, 	//F6 key
	F7 			= 0x76, 	//F7 key
	F8 			= 0x77, 	//F8 key
	F9 			= 0x78, 	//F9 key
	F10 		= 0x79, 	//F10 key
	F11 		= 0x7A, 	//F11 key
	F12 		= 0x7B, 	//F12 key
	F13 		= 0x7C, 	//F13 key
	F14 		= 0x7D, 	//F14 key
	F15 		= 0x7E, 	//F15 key
	F16 		= 0x7F, 	//F16 key
	F17 		= 0x80, 	//F17 key
	F18 		= 0x81, 	//F18 key
	F19 		= 0x82, 	//F19 key
	F20 		= 0x83, 	//F20 key
	F21 		= 0x84, 	//F21 key
	F22 		= 0x85, 	//F22 key
	F23 		= 0x86, 	//F23 key
	F24 		= 0x87, 	//F24 key
	NUMLOCK 	= 0x90, 	//NUM LOCK key
	SCROLL 	    = 0x91, 	//SCROLL LOCK key
	//- = 0x92 - 96 	OEM specific
	SHIFT_L 	= 0xA0, 	//Left SHIFT key
	SHIFT_R 	= 0xA1, 	//Right SHIFT key
	CONTROL_L 	= 0xA2, 	//Left CONTROL key
	CONTROL_R 	= 0xA3, 	//Right CONTROL key
	ALT_L 		= 0xA4, 	//Left ALT key
	ALT_R 		= 0xA5, 	//Right ALT key
	//VK_BROWSER_BACK 	= 0xA6 	Browser Back key
	//VK_BROWSER_FORWARD 	= 0xA7 	Browser Forward key
	//VK_BROWSER_REFRESH 	= 0xA8 	Browser Refresh key
	//VK_BROWSER_STOP 	= 0xA9 	Browser Stop key
	//VK_BROWSER_SEARCH 	= 0xAA 	Browser Search key
	//VK_BROWSER_FAVORITES 	= 0xAB 	Browser Favorites key
	//VK_BROWSER_HOME 	= 0xAC 	Browser Start and Home key
	VOLUME_MUTE = 0xAD, 	//Volume Mute key
	VOLUME_DOWN = 0xAE, 	//Volume Down key
	VOLUME_UP 	= 0xAF, 	//Volume Up key
	//VK_MEDIA_NEXT_TRACK 	= 0xB0 	Next Track key
	//VK_MEDIA_PREV_TRACK 	= 0xB1 	Previous Track key
	//VK_MEDIA_STOP 	= 0xB2 	Stop Media key
	//VK_MEDIA_PLAY_PAUSE 	= 0xB3 	Play / Pause Media key
	//VK_LAUNCH_MAIL 	= 0xB4 	Start Mail key
	//VK_LAUNCH_MEDIA_SELECT 	= 0xB5 	Select Media key
	//VK_LAUNCH_APP1 	= 0xB6 	Start Application 1 key
	//VK_LAUNCH_APP2 	= 0xB7 	Start Application 2 key
	
	//VK_OEM_1 	= 0xBA 	Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the;: key
	//VK_OEM_PLUS 	= 0xBB 	For any country / region, the + key
	//VK_OEM_COMMA 	= 0xBC 	For any country / region, the, key
	//VK_OEM_MINUS 	= 0xBD 	For any country / region, the - key
	//VK_OEM_PERIOD 	= 0xBE 	For any country / region, the.key
	//VK_OEM_2 	= 0xBF 	Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the / ? key
	//VK_OEM_3 	= 0xC0 	Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the `~ key
	
	//VK_OEM_4 	= 0xDB 	Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the[{ key
	//VK_OEM_5 	= 0xDC 	Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the \\ | key
	//VK_OEM_6 	= 0xDD 	Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the]} key
	//VK_OEM_7 	= 0xDE 	Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the '" key
	//VK_OEM_8 	= 0xDF 	Used for miscellaneous characters; it can vary by keyboard.
	
	//- = 0xE1 	OEM specific
	//VK_OEM_102 	= 0xE2 	The <> keys on the US standard keyboard, or the \\ | key on the non - US 102 - key keyboard
	//- = 0xE3 - E4 	OEM specific
	//VK_PROCESSKEY 	= 0xE5 	IME PROCESS key
	//- = 0xE6 	OEM specific
	//VK_PACKET 	= 0xE7 	Used to pass Unicode characters as if they were keystrokes.The VK_PACKET key is the low word of a 32 - bit Virtual Key value used for non - keyboard input methods.For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
	
	//- = 0xE9 - F5 	OEM specific
	//VK_ATTN 	= 0xF6 	Attn key
	//VK_CRSEL 	= 0xF7 	CrSel key
	//VK_EXSEL 	= 0xF8 	ExSel key
	//VK_EREOF 	= 0xF9 	Erase EOF key
	//VK_PLAY 	= 0xFA 	Play key
	//VK_ZOOM 	= 0xFB 	Zoom key
	//VK_NONAME 	= 0xFC 	Reserved
	//VK_PA1 	= 0xFD 	PA1 key
	//VK_OEM_CLEAR 	= 0xFE 	Clear key
};

#endif INPUT_AND_WINDOW_H
