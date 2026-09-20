/* date = September 12th 2026 5:15 pm */

#ifndef HELIOS_PLATFORM_H
#define HELIOS_PLATFORM_H

#include "helios_math.h"

#if HELIOS_CPP && 0
extern "C"
{
#endif
  
  /**
* TODO(Sebas): Services that the platform layer provides to the game.
*/
  
#if HELIOS_DEBUG
  struct debug_read_file_result
  {
    u32 contentsSize;
    void* contents;
  };
  function debug_read_file_result DEBUGPlatformReadEntireFile(char* fileName);
  function b32  DEBUGPlatformWriteEntireFile(char* fileName, u64 memorySize, void* memory);
  function void DEBUGPlatformFreeFileMemory(void* memory);
#else
#endif
  
  /**
* NOTE(Sebas): Services that the game provides to the platform layer.
*/
  
#define MAX_FRAME_EVENTS 64
#define MAX_TEXT_INPUT 32
  
  enum game_input_keycode {
    // Mouse buttons
    HS_KEY_NIL = 0,
    HS_KEY_LBUTTON        = 0x01,
    HS_KEY_RBUTTON        = 0x02,
    HS_KEY_CANCEL         = 0x03,
    HS_KEY_MBUTTON        = 0x04,
    HS_KEY_XBUTTON1       = 0x05,
    HS_KEY_XBUTTON2       = 0x06,
    
    // Architectural keys
    HS_KEY_BACK           = 0x08,
    HS_KEY_TAB            = 0x09,
    HS_KEY_CLEAR          = 0x0C,
    HS_KEY_RETURN         = 0x0D,
    
    // Modifiers
    HS_KEY_SHIFT          = 0x10,
    HS_KEY_CONTROL        = 0x11,
    HS_KEY_MENU           = 0x12, // Alt Key
    HS_KEY_PAUSE          = 0x13,
    HS_KEY_CAPITAL        = 0x14, // Caps Lock
    
    // IME / Language keys
    HS_KEY_KANA           = 0x15,
    HS_KEY_HANGEUL        = 0x15,
    HS_KEY_HANGUL         = 0x15,
    HS_KEY_IME_ON         = 0x16,
    HS_KEY_JUNJA          = 0x17,
    HS_KEY_FINAL          = 0x18,
    HS_KEY_HANJA          = 0x19,
    HS_KEY_KANJI          = 0x19,
    HS_KEY_IME_OFF        = 0x1A,
    
    // System navigation
    HS_KEY_ESCAPE         = 0x1B,
    HS_KEY_CONVERT        = 0x1C,
    HS_KEY_NONCONVERT     = 0x1D,
    HS_KEY_ACCEPT         = 0x1E,
    HS_KEY_MODECHANGE     = 0x1F,
    HS_KEY_SPACE          = 0x20,
    HS_KEY_PRIOR          = 0x21, // Page Up
    HS_KEY_NEXT           = 0x22, // Page Down
    HS_KEY_END            = 0x23,
    HS_KEY_HOME           = 0x24,
    
    // Arrow keys
    HS_KEY_LEFT           = 0x25,
    HS_KEY_UP             = 0x26,
    HS_KEY_RIGHT          = 0x27,
    HS_KEY_DOWN           = 0x28,
    
    // Interactive commands
    HS_KEY_SELECT         = 0x29,
    HS_KEY_PRINT          = 0x2A,
    HS_KEY_EXECUTE        = 0x2B,
    HS_KEY_SNAPSHOT       = 0x2C, // Print Screen
    HS_KEY_INSERT         = 0x2D,
    HS_KEY_DELETE         = 0x2E,
    HS_KEY_HELP           = 0x2F,
    
    // Number row (Alpha-numeric standard ASCII mapping)
    HS_KEY_0              = 0x30,
    HS_KEY_1              = 0x31,
    HS_KEY_2              = 0x32,
    HS_KEY_3              = 0x33,
    HS_KEY_4              = 0x34,
    HS_KEY_5              = 0x35,
    HS_KEY_6              = 0x36,
    HS_KEY_7              = 0x37,
    HS_KEY_8              = 0x38,
    HS_KEY_9              = 0x39,
    
    // Alphabet keys (Standard ASCII mapping)
    HS_KEY_A              = 0x41,
    HS_KEY_B              = 0x42,
    HS_KEY_C              = 0x43,
    HS_KEY_D              = 0x44,
    HS_KEY_E              = 0x45,
    HS_KEY_F              = 0x46,
    HS_KEY_G              = 0x47,
    HS_KEY_H              = 0x48,
    HS_KEY_I              = 0x49,
    HS_KEY_J              = 0x4A,
    HS_KEY_K              = 0x4B,
    HS_KEY_L              = 0x4C,
    HS_KEY_M              = 0x4D,
    HS_KEY_N              = 0x4E,
    HS_KEY_O              = 0x4F,
    HS_KEY_P              = 0x50,
    HS_KEY_Q              = 0x51,
    HS_KEY_R              = 0x52,
    HS_KEY_S              = 0x53,
    HS_KEY_T              = 0x54,
    HS_KEY_U              = 0x55,
    HS_KEY_V              = 0x56,
    HS_KEY_W              = 0x57,
    HS_KEY_X              = 0x58,
    HS_KEY_Y              = 0x59,
    HS_KEY_Z              = 0x5A,
    
    // OS Command keys
    HS_KEY_LWIN           = 0x5B,
    HS_KEY_RWIN           = 0x5C,
    HS_KEY_APPS           = 0x5D,
    HS_KEY_SLEEP          = 0x5F,
    
    // Numpad inputs
    HS_KEY_NUMPAD0        = 0x60,
    HS_KEY_NUMPAD1        = 0x61,
    HS_KEY_NUMPAD2        = 0x62,
    HS_KEY_NUMPAD3        = 0x63,
    HS_KEY_NUMPAD4        = 0x64,
    HS_KEY_NUMPAD5        = 0x65,
    HS_KEY_NUMPAD6        = 0x66,
    HS_KEY_NUMPAD7        = 0x67,
    HS_KEY_NUMPAD8        = 0x68,
    HS_KEY_NUMPAD9        = 0x69,
    HS_KEY_MULTIPLY       = 0x6A,
    HS_KEY_ADD            = 0x6B,
    HS_KEY_SEPARATOR      = 0x6C,
    HS_KEY_SUBTRACT       = 0x6D,
    HS_KEY_DECIMAL        = 0x6E,
    HS_KEY_DIVIDE         = 0x6F,
    
    // Function keys
    HS_KEY_F1             = 0x70,
    HS_KEY_F2             = 0x71,
    HS_KEY_F3             = 0x72,
    HS_KEY_F4             = 0x73,
    HS_KEY_F5             = 0x74,
    HS_KEY_F6             = 0x75,
    HS_KEY_F7             = 0x76,
    HS_KEY_F8             = 0x77,
    HS_KEY_F9             = 0x78,
    HS_KEY_F10            = 0x79,
    HS_KEY_F11            = 0x7A,
    HS_KEY_F12            = 0x7B,
    HS_KEY_F13            = 0x7C,
    HS_KEY_F14            = 0x7D,
    HS_KEY_F15            = 0x7E,
    HS_KEY_F16            = 0x7F,
    HS_KEY_F17            = 0x80,
    HS_KEY_F18            = 0x81,
    HS_KEY_F19            = 0x82,
    HS_KEY_F20            = 0x83,
    HS_KEY_F21            = 0x84,
    HS_KEY_F22            = 0x85,
    HS_KEY_F23            = 0x86,
    HS_KEY_F24            = 0x87,
    
    // Control toggles
    HS_KEY_NUMLOCK        = 0x90,
    HS_KEY_SCROLL         = 0x91,
    
    // Explicit side-specific modifier codes
    HS_KEY_LSHIFT         = 0xA0,
    HS_KEY_RSHIFT         = 0xA1,
    HS_KEY_LCONTROL       = 0xA2,
    HS_KEY_RCONTROL       = 0xA3,
    HS_KEY_LMENU          = 0xA4, // Left Alt
    HS_KEY_RMENU          = 0xA5, // Right Alt
    
    // Browser navigation (Often on multimedia keyboards)
    HS_KEY_BROWSER_BACK        = 0xA6,
    HS_KEY_BROWSER_FORWARD     = 0xA7,
    HS_KEY_BROWSER_REFRESH     = 0xA8,
    HS_KEY_BROWSER_STOP        = 0xA9,
    HS_KEY_BROWSER_SEARCH      = 0xAA,
    HS_KEY_BROWSER_FAVORITES   = 0xAB,
    HS_KEY_BROWSER_HOME        = 0xAC,
    
    // Volume/Media settings
    HS_KEY_VOLUME_MUTE         = 0xAD,
    HS_KEY_VOLUME_DOWN         = 0xAE,
    HS_KEY_VOLUME_UP           = 0xAF,
    HS_KEY_MEDIA_NEXT_TRACK    = 0xB0,
    HS_KEY_MEDIA_PREV_TRACK    = 0xB1,
    HS_KEY_MEDIA_STOP          = 0xB2,
    HS_KEY_MEDIA_PLAY_PAUSE    = 0xB3,
    HS_KEY_LAUNCH_MAIL         = 0xB4,
    HS_KEY_LAUNCH_MEDIA_SELECT = 0xB5,
    HS_KEY_LAUNCH_APP1         = 0xB6,
    HS_KEY_LAUNCH_APP2         = 0xB7,
    
    // OEM Punctuations (Regional variation layout keys)
    HS_KEY_OEM_1          = 0xBA, // ';:' key for US
    HS_KEY_OEM_PLUS       = 0xBB, // '+' key
    HS_KEY_OEM_COMMA      = 0xBC, // ',' key
    HS_KEY_OEM_MINUS      = 0xBD, // '-' key
    HS_KEY_OEM_PERIOD     = 0xBE, // '.' key
    HS_KEY_OEM_2          = 0xBF, // '/?' key for US
    HS_KEY_OEM_3          = 0xC0, // '`~' key for US
    HS_KEY_OEM_4          = 0xDB, // '[{' key for US
    HS_KEY_OEM_5          = 0xDC, // '\|' key for US
    HS_KEY_OEM_6          = 0xDD, // ']}' key for US
    HS_KEY_OEM_7          = 0xDE, // ''"' key for US
    HS_KEY_OEM_8          = 0xDF,
    HS_KEY_OEM_102        = 0xE2, // Angle bracket/backslash on non-US keyboards
    
    // Core utilities
    HS_KEY_PROCESSKEY     = 0xE5,
    HS_KEY_PACKET         = 0xE7, // Used to pass Unicode chars via text events
    HS_KEY_ATTN           = 0xF6,
    HS_KEY_CRSEL          = 0xF7,
    HS_KEY_EXSEL          = 0xF8,
    HS_KEY_EREOF          = 0xF9,
    HS_KEY_PLAY           = 0xFA,
    HS_KEY_ZOOM           = 0xFB,
    HS_KEY_NONAME         = 0xFC,
    HS_KEY_PA1            = 0xFD,
    HS_KEY_OEM_CLEAR      = 0xFE
  };
  
  enum game_input_event_type
  {
    INPUT_EVENT_NIL=0,
    INPUT_EVENT_KEY_DOWN,
    INPUT_EVENT_KEY_UP,
    INPUT_EVENT_MOUSE_DOWN,
    INPUT_EVENT_MOUSE_UP,
    INPUT_EVENT_COUNT,
  };
  
  enum game_input_modifiers
  {
    INPUT_MODS_NONE = 0,
    INPUT_MODS_SHIFT = (1 << 0),
    INPUT_MODS_CTRL = (1 << 1),
    INPUT_MODS_ALT = (1 << 2),
  };
  
  struct game_input_event
  {
    game_input_event_type type;
    game_input_keycode code;
    f32 mouseX;
    f32 mouseY;
    game_input_modifiers currentModifiers;
  };
  
  struct game_button_state
  {
    b32 endedDown;
    b32 transitionCount;
  };
  
  struct game_keyboard_input
  {
    f32 mouseX;
    f32 mouseY;
    game_button_state buttons[256];
    
    u32 eventCount;
    game_input_event events[MAX_FRAME_EVENTS];
    
    u32 textLength;
    char textInput[MAX_TEXT_INPUT];
  };
  
  struct game_controller_input
  {
    b32 isConnected;
    union
    {
      game_button_state buttons[10];
      struct
      {
        game_button_state padUp;
        game_button_state padDown;
        game_button_state padLeft;
        game_button_state padRight;
        game_button_state buttonUp;
        game_button_state buttonDown;
        game_button_state buttonLeft;
        game_button_state buttonRight;
        game_button_state shoulderLeft;
        game_button_state shoulderRight;
      };
      
      f32 triggerLeft;
      f32 triggerRight;
    };
  };
  
  struct game_input
  {
    b32 isController;
    f32 deltaTime;
    game_keyboard_input keyboard;
    game_controller_input controllers[4];
  };
  
  struct game_frame_buffer
  {
    void* memory;
    s32 width;
    s32 height;
    s32 pitch;
    s32 bytesPerPixel;
  };
  
  //~ NOTE(Sebas): REQUIRED to be cleared to zero at startup
  struct game_memory
  {
    b32 isInitialized;
    u64 permanentMemorySize;
    void* permanentMemory;
    u64 transientMemorySize;
    void* transientMemory;
  };
  
  function void GameUpdateAndRender(game_memory* memory, game_frame_buffer* buffer, game_input* input);
  
#if HELIOS_CPP && 0
}
#endif
#endif //HELIOS_PLATFORM_H
