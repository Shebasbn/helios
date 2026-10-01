/* date = September 12th 2026 4:17 pm */

#ifndef HELIOS_H
#define HELIOS_H

enum game_colour_format
{
  FMT_ARGB,
  FMT_RGBA,
  FMT_PADRGB,
  FMT_RGBPAD,
};

struct game_colour
{
  f32 alpha;
  f32 red;
  f32 green;
  f32 blue;
  game_colour_format format;
};

struct game_camera
{
  vec2_f64 systemHUPos;
  vec2_f32 screenPixelPos;
  vec2_f64 targetSystemHUPos;
  vec2_f64 deltaSystemHUPos;
  
  u32 newTargetBodyID;
  u32 oldTargetBodyID;
  s32 zoomLevel;
  s32 zoomLevelMin;
  s32 zoomLevelMax;
  f32 zoomBase;
  
  f64 systemDeltaX;
  f64 systemDeltaY;
  
  b32 isMoving;
  b32 isZooming;
  
  f32 defaultPixelsPerHU;
  //vec2_f32 screenDim;
  f32 metersPerHU;
  f32 scalePixelsPerHU;
  f32 targetScalePixelsPerHU;
};

struct memory_arena
{
  void* memory;
  memory_index offset;
  u64 size;
};

function void ArenaInit(memory_arena* arena, u64 size, void* memory);
function void* ArenaPush(memory_arena* arena, u64 size);
#define PushType(arena, type, count) (type*)ArenaPush(arena, sizeof(type) * count)
#define PushStruct(arena, type) (type*)ArenaPush(arena, sizeof(type))
#define PushArray(arena, array) ArenaPush(arena, sizeof(array))
#define PushSize(arena, size, count) ArenaPush(arena, size * count)

#include "helios_body.h"

struct galaxy
{
  u32 minSystemGenCount;
  u32 maxSystemGenCount;
  s32 systemCount;
  star_system* systems;
};

struct game_bitmap
{
  void* memory;
  s32 width;
  s32 height;
  s32 pitch;
  s32 bytesPerPixel;
};

#define GAME_EXPORT no_name_mangle
#define GAME_TEXT_BUFFER_MAX_SIZE 256
struct game_state
{
  game_camera camera;
  
  game_colour mouseDownColour;
  game_colour mouseUpColour;
  game_colour mouseCursorColour;
  b32 drawCursor;
  b32 mouseCursorColourChanged;
  vec2_f32 mouseScreenPosPixels;
  vec2_f32 mouseWorldPosHU;
  
  memory_arena galaxyArena;
  galaxy Galaxy;
  u32 currentSystemID;
  
  u64 accumulatorTicks;
  
  game_bitmap bitmap;
  
  s32 textLength;
  char textBuffer[GAME_TEXT_BUFFER_MAX_SIZE];
};



#endif //HELIOS_H