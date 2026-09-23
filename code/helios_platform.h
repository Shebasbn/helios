/* date = September 12th 2026 5:15 pm */

#ifndef HELIOS_PLATFORM_H
#define HELIOS_PLATFORM_H

#include "helios_math.h"

#if HELIOS_CPP 
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
  
  
# define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void* memory)
  typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);
  
# define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(char* fileName)
  typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);
  
# define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(char* fileName, u64 memorySize, void* memory)
  typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
  
#endif
  
  /**
* NOTE(Sebas): Services that the game provides to the platform layer.
*/
  
#define MAX_FRAME_EVENTS 64
#define MAX_TEXT_INPUT 32
  
  enum game_input_keycode 
  {
#include "helios_keybinds_table.inl"
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
    game_input_modifiers modifiers;
    b32 isProcessed;
  };
  
  struct game_button_state
  {
    b32 endedDown;
    b32 transitionCount;
  };
  
  struct game_analog_state
  {
    vec2 unitVector;
    f32 normalizedMagnitude;
  };
  
  struct game_keyboard_input
  {
    f32 mouseX;
    f32 mouseY;
    game_button_state buttons[256];
    
    game_input_modifiers currentModifiers;
    
    u32 eventCount;
    game_input_event events[MAX_FRAME_EVENTS];
    
    u32 textLength;
    char textInput[MAX_TEXT_INPUT];
  };
  
  struct game_controller_input
  {
    b32 isConnected;
    b32 isAnalog;
    u32 packetNumber;
    union
    {
      game_analog_state stickLeft;
      struct
      {
        union
        {
          vec2 stickLeftVec;
          struct
          {
            f32 stickLeftX;
            f32 stickLeftY;
          };
        };
        f32 stickLeftMagnitude;
      };
    };
    
    union
    {
      game_analog_state stickRight;
      struct
      {
        union
        {
          vec2 stickRightVec;
          struct
          {
            f32 stickRightX;
            f32 stickRightY;
          };
        };
        f32 stickRightMagnitude;
      };
    };
    
    union
    {
      game_button_state buttons[12];
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
        
        game_button_state back;
        game_button_state start;
        
        game_button_state terminator;
      };
      
      f32 triggerLeft;
      f32 triggerRight;
    };
  };
  
  struct game_input
  {
    b32 isController;
    f32 deltaTime;
    u32 accumulater;
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
    
#if HELIOS_DEBUG
    debug_platform_read_entire_file* DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file* DEBUGPlatformWriteEntireFile;
    debug_platform_free_file_memory* DEBUGPlatformFreeFileMemory;
#endif
  };
  
# define GAME_UPDATE_AND_RENDER(name) void name(game_memory* memory, game_frame_buffer* buffer, game_input* inputState)
  typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
  
  function inline game_controller_input* GetController(game_input* input, u32 controllerIndex)
  {
    HS_Assert(controllerIndex < HS_ArrayCount(input->controllers));
    return &input->controllers[controllerIndex];
  }
  
  
  
  
#if HELIOS_CPP
}
#endif
#endif //HELIOS_PLATFORM_H
