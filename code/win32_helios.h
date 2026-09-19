/* date = September 17th 2026 8:59 am */

#ifndef WIN32_HELIOS_H
#define WIN32_HELIOS_H

#include "helios_platform.h"

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
};

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

struct win32_dimension
{
  s32 width;
  s32 height;
};


#endif //WIN32_HELIOS_H
