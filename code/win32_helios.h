/* date = September 17th 2026 8:59 am */

#ifndef WIN32_HELIOS_H
#define WIN32_HELIOS_H

#include "helios_platform.h"

#define BUFFER_WIDTH 960 
#define BUFFER_HEIGHT 540 

struct win32_frame_buffer
{
  union
  {
    game_frame_buffer gameFrameBuffer;
    struct
    {
      void* memory;
      s32 width;
      s32 height;
      s32 pitch;
      s32 bytesPerPixel;
    };
  };
  BITMAPINFO info;
};

struct win32_window
{
  HINSTANCE instance;
  HWND handle;
  s32 x;
  s32 y;
  s32 width;
  s32 height;
  DWORD style;
  DWORD exStyle;
  b32 isResizing;
  b32 isRunning;
  win32_frame_buffer frameBuffer;
  
  b32 isActive;
};

struct win32_dimension
{
  s32 width;
  s32 height;
};

struct win32_game_code
{
  HMODULE gameCodeDLL;
  FILETIME dllLastWriteTime;
  game_update_and_render* UpdateAndRender;
  
  b32 isValid;
};

struct win32_state
{
  win32_window window;
  
  HANDLE gameMemoryHandle;
  u64 gameMemorySize;
  void* gameMemoryBlock;
  
  HANDLE recordingHandle;
  s32 inputRecordingIndex;
  
  HANDLE playbackHandle;
  s32 inputPlaybackIndex;
  
  b32 resetInput;
};


#endif //WIN32_HELIOS_H
