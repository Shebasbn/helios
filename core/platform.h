/* date = May 27th 2026 6:20 pm */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <common.h>
//#include <core_string.h>

////////////////////////////////////////////////////////////////
//~ Sebas: Platform System 

typedef struct PlatformState PlatformState;

struct PlatformHandle
{
    PlatformState* state;
};


B32 PF_StartUp(Arena* arena, PlatformHandle* handle, String8 app_name, S32 x, S32 y, S32 width, S32 height);
void PF_Shutdown(Arena* arena, PlatformHandle handle);

U64 PF_PageSize(void);
U64 PF_AllocGranularity(void);

void* PF_MemoryReserve(U64 size);
void PF_MemoryCommit(void* ptr, U64 size);
void PF_MemoryDecommit(void* ptr, U64 size);
void PF_MemoryRelease(void* ptr);

void PF_ConsoleWrite(String8 message, U8 colour);

////////////////////////////////////////////////////////////////
//~ Sebas: Platform Time Types and Functions

struct TimeStamp
{
#if PLATFORM_WINDOWS
    U64 counter;
#else
#endif
};

struct PF_DateTime
{
    U16 year;         // e.g., 2026
    U8  month;        // 1 - 12 (January = 1)
    U8  day;          // 1 - 31
    U8  hour;         // 0 - 23
    U8  minute;       // 0 - 59
    U8  second;       // 0 - 59
    U16 millisecond;  // 0 - 999 (Crucial for high-accuracy log files)
};

TimeStamp PF_TimeStampCreate(void);
F64 PF_TimeGetHardwareFrequency(void);
F64 PF_TimeDeltaInSeconds(TimeStamp start, TimeStamp end);
F64 PF_TimeSinceStartUpSec(void);
void PF_TimeGetSystemTime(PF_DateTime* out_time);

void PF_TimeSleep(F64 seconds);

////////////////////////////////////////////////////////////////
//~ Sebas: Platform Event System 

enum PF_KeyCode
{
    PF_KEY_NILL = 0,
    PF_KEY_A = 1,
    PF_KEY_B,
    PF_KEY_C,
    PF_KEY_D,
    PF_KEY_E,
    PF_KEY_F,
    PF_KEY_G,
    PF_KEY_H,
    PF_KEY_I,
    PF_KEY_J,
    PF_KEY_K,
    PF_KEY_L,
    PF_KEY_M,
    PF_KEY_N,
    PF_KEY_O,
    PF_KEY_P,
    PF_KEY_Q,
    PF_KEY_R,
    PF_KEY_S,
    PF_KEY_T,
    PF_KEY_U,
    PF_KEY_V,
    PF_KEY_W,
    PF_KEY_X,
    PF_KEY_Y,
    PF_KEY_Z, // Automatically increments to 26
    
    // --- Number Row (0 - 9) ---
    PF_KEY_0 = 30,
    PF_KEY_1,
    PF_KEY_2,
    PF_KEY_3,
    PF_KEY_4,
    PF_KEY_5,
    PF_KEY_6,
    PF_KEY_7,
    PF_KEY_8,
    PF_KEY_9, // Increments to 39
    
    // --- Punctuation & Symbols ---
    PF_KEY_GRAVE = 40,      // ` or ~
    PF_KEY_MINUS,          // -
    PF_KEY_EQUAL,          // =
    PF_KEY_LBRACKET,       // [
    PF_KEY_RBRACKET,       // ]
    PF_KEY_BACKSLASH,      // 
    PF_KEY_SEMICOLON,      // ;
    PF_KEY_APOSTROPHE,     // '
    PF_KEY_COMMA,          // ,
    PF_KEY_PERIOD,         // .
    PF_KEY_SLASH,          // /
    
    // --- Controls & Modifiers ---
    PF_KEY_ESCAPE = 100,
    PF_KEY_ENTER,
    PF_KEY_SPACE,
    PF_KEY_BACKSPACE,
    PF_KEY_TAB,
    PF_KEY_LSHIFT,
    PF_KEY_RSHIFT,
    PF_KEY_LCTRL,
    PF_KEY_RCTRL,
    PF_KEY_LALT,
    PF_KEY_RALT,
    PF_KEY_LWIN,           // Windows Key / Command / Super
    PF_KEY_RWIN,
    PF_KEY_CAPSLOCK,
    PF_KEY_PRINTSCREEN,
    PF_KEY_SCROLLLOCK,
    PF_KEY_PAUSE,
    
    // --- Editing & Navigation ---
    PF_KEY_UP = 200,
    PF_KEY_DOWN,
    PF_KEY_LEFT,
    PF_KEY_RIGHT,
    PF_KEY_INSERT,
    PF_KEY_DELETE,
    PF_KEY_HOME,
    PF_KEY_END,
    PF_KEY_PAGEUP,
    PF_KEY_PAGEDOWN,
    
    // --- Function Keys ---
    PF_KEY_F1 = 250,
    PF_KEY_F2,
    PF_KEY_F3,
    PF_KEY_F4,
    PF_KEY_F5,
    PF_KEY_F6,
    PF_KEY_F7,
    PF_KEY_F8,
    PF_KEY_F9,
    PF_KEY_F10,
    PF_KEY_F11,
    PF_KEY_F12,
    
    // --- Numeric Keypad (Numpad) ---
    PF_KEY_NUM_LOCK = 300,
    PF_KEY_NUM_0,
    PF_KEY_NUM_1,
    PF_KEY_NUM_2,
    PF_KEY_NUM_3,
    PF_KEY_NUM_4,
    PF_KEY_NUM_5,
    PF_KEY_NUM_6,
    PF_KEY_NUM_7,
    PF_KEY_NUM_8,
    PF_KEY_NUM_9,
    PF_KEY_NUM_DIVIDE,     // Numpad /
    PF_KEY_NUM_MULTIPLY,   // Numpad *
    PF_KEY_NUM_SUBTRACT,   // Numpad -
    PF_KEY_NUM_ADD,        // Numpad +
    PF_KEY_NUM_ENTER,      // Numpad Enter
    PF_KEY_NUM_DECIMAL     // Numpad .
};

enum PF_GamepadCode {
    PF_GAMEPAD_NILL = 0,
    // Pointer Axes
    /*PF_POINTER_AXIS_SCROLL_X = 1, // Horizontal scroll (Trackpads/Tilt-wheels)
    PF_POINTER_AXIS_SCROLL_Y = 2, // Vertical scroll (Standard mouse wheel zoom)
    */
    // Gamepad Axes
    PF_GAMEPAD_AXIS_LEFT_X   = 10,
    PF_GAMEPAD_AXIS_LEFT_Y   = 11,
    PF_GAMEPAD_AXIS_RIGHT_X  = 12,
    PF_GAMEPAD_AXIS_RIGHT_Y  = 13
};

enum PF_PointerCode
{
    PF_POINTER_NILL = 0,
    
    // --- Digital Button Inputs (PF_EVENT_POINTER_DOWN / UP) ---
    // Standard Desktop Mouse Mapping
    PF_POINTER_BUTTON_LEFT   = 1,
    PF_POINTER_BUTTON_RIGHT  = 2,
    PF_POINTER_BUTTON_MIDDLE = 3,
    PF_POINTER_BUTTON_X1     = 4, // Side button 1
    PF_POINTER_BUTTON_X2     = 5, // Side button 2
    
    // Touchscreen & Steam Deck Trackpad Click Mapping
    // (Maps touch inputs or trackpad physical clicks cleanly into the pointer system)
    PF_POINTER_FINGER_PRIMARY   = 1, // Treat tap as left click
    PF_POINTER_FINGER_SECONDARY = 2, // Multi-touch finger 2
    PF_POINTER_FINGER_TERTIARY  = 3, // Multi-touch finger 3
    
    // --- Analog Continuous Axes (PF_EVENT_POINTER_AXIS_MOVE) ---
    PF_POINTER_AXIS_SCROLL_X = 20, // Horizontal trackpad scroll / mouse tilt wheel
    PF_POINTER_AXIS_SCROLL_Y = 21, // Standard vertical mouse scroll wheel (Zoom)
    PF_POINTER_AXIS_PRESSURE = 22  // For pressure-sensitive touch screens or styluses
};

enum PF_EventKind 
{
    PF_EVENT_NILL = 0,
    PF_EVENT_GAME_SHUTDOWN,
    // Surface States
    PF_EVENT_SURFACE_RESIZED,
    PF_EVENT_SURFACE_FOCUS_LOST,
    PF_EVENT_SURFACE_FOCUS_GAINED,
    /*PF_EVENT_SURFACE_ZOOM_IN_OR_NAV_UP,
    PF_EVENT_SURFACE_ZOOM_OUT_OR_NAV_DOWN,*/
    
    // Keyboard States
    PF_EVENT_KEY_PRESSED,
    PF_EVENT_KEY_RELEASED,
    
    PF_EVENT_TEXT_INPUT,
    
    // Pointer States
    PF_EVENT_POINTER_MOVE,
    PF_EVENT_POINTER_DOWN,
    PF_EVENT_POINTER_UP,
    PF_EVENT_POINTER_AXIS_MOVE,
    
    // Gamepad States
    PF_EVENT_GAMEPAD_BUTTON_DOWN,
    PF_EVENT_GAMEPAD_BUTTON_UP,
    PF_EVENT_GAMEPAD_AXIS_MOVE,
};

enum PF_Modifiers
{
    PF_MOD_CTRL = (1<<0),
    PF_MOD_SHIFT = (1<<1),
    PF_MOD_ALT = (1<<2),
};

struct PF_KeyPayload
{
    PF_Modifiers mods;
};

struct PF_AnalogPayload
{
    F32 value;
};

struct PF_PointerPayload
{
    F32 x;
    F32 y;
    F32 delta_x;
    F32 delta_y;
};

struct PF_SurfacePayload
{
    U32 width;
    U32 height;
};

struct PF_TextPayload
{
    char utf8_text[32];
};

typedef U32 InputCode;

struct PF_Event
{
    PF_Event* next;
    PF_Event* prev;
    
    PF_EventKind kind;
    bool handled;
    TimeStamp timestamp;
    InputCode input_code;
    
    union
    {
        PF_KeyPayload key;
        PF_PointerPayload pointer;
        PF_SurfacePayload surface;
        PF_AnalogPayload analog;
        PF_TextPayload text;
    };
};


struct PF_EventList
{
    PF_Event* first;
    PF_Event* last;
    U32 count;
};

B32 PF_ProcessPFEvents(Arena* arena, PlatformHandle handle, PF_EventList* list);


////////////////////////////////////////////////////////////////
//~ Sebas: Mulitthreading

//struct PF_ThreadHandle
//{
//void* thread;
//};

void PF_CreateThread(Arena* arena);

#endif //PLATFORM_H
