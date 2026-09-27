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
  f32 red;
  f32 green;
  f32 blue;
  f32 alpha;
  game_colour_format format;
};

struct game_camera
{
  vec2_f64 worldHUPos;
  vec2_f32 screenPixelPos;
  vec2_f64 targetWorldHUPos;
  s32 zoomLevel;
  s32 zoomLevelMin;
  s32 zoomLevelMax;
  f32 zoomBase;
  
  b32 isMoving;
  //b32 isZooming;
  
  f32 defaultPixelsPerHU;
  //vec2_f32 screenDim;
  f32 metersPerHU;
  f32 scalePixelsPerHU;
  f32 targetScalePixelsPerHU;
};

enum class system_body_type
{
  Nil = 0,
  Star,
  Planet,
  Moon,
  Satellite,
  Ship,
};

struct system_body 
{
  u32 ID;
  u32 parentID;
  u32 firstChildID;
  u32 nextID;
  u32 prevID;
  system_body_type type;
  vec2_f32 worldRelHUPos;
  vec2_f64 worldHUPos;
  f64 worldHURadius;
  vec2_f32 screenPixelPos;
  f32 screenPixelRadius;
  game_colour colour;
  char* name;
  b32 shouldRender;
};


#define MAX_SYSTEM_BODY_COUNT 50
#define NilID (u32)system_body_type::Nil
struct star_system
{
  system_body bodies[MAX_SYSTEM_BODY_COUNT];
  s32 bodyCount;
  u32 rootBodyID;
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
  
  s32 textLength;
  char textBuffer[GAME_TEXT_BUFFER_MAX_SIZE];
};



#endif //HELIOS_H