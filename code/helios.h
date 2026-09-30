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

enum class system_body_type
{
  Nil = 0,
  Star,
  Planet,
  Moon,
  AsteroidBelt,
};

//struct body_id
//{
//body_id parentID;
//u32 relativeID;
//};

struct body_generation_info
{
  char* name;
  system_body_type type;
  b32 isRoot;
  game_colour colour;
  f64 mass;
  f64 radiusM;
  f64 avgDistanceFromParent;
  f64 argumentOfPeriapsisDeg;
  f64 eccentricity;
};

struct system_generation_info
{
  body_generation_info* systemGenBodies;
  u32 systemGenBodyCount;
};

struct keplerian_body 
{
  // Permanent Orbital Elements (Set once at spawn)
  //f64 gravityParameter; // G * Mass of Parent
  f64 semiMajorAxis;
  f64 semiMinorAxis;
  f64 eccentricity;            // Orbit shape (0 = perfect circle)
  f64 meanMotion;
  f64 argumentOfPeriapsis;
  
  // Dynamic State Variables (Changes every frame)
  f64 meanAnomaly;            // Track progress along the orbit
  vec2_f64 position;              // Current position in meters
};

struct system_body 
{
  u32 bodyID;
  u32 parentID;
  
  system_body_type type;
  //vec2_f32 worldRelHUPos;
  //vec2_f64 worldHUPos;
  
  keplerian_body keplerBody;
  
  vec2_f64 nextSystemHUPos;
  vec2_f64 systemHUPos;
  //vec2_f32 bodyRelHUPos;
  vec2_f32 screenPixelPos;
  
  f64 radiusHU;
  f64 mass;
  f32 screenPixelRadius;
  game_colour colour;
  char* name;
  b32 shouldRender;
};


#define MAX_SYSTEM_BODY_COUNT 50
#define NilID (u32)system_body_type::Nil
struct star_system
{
  u32 systemID;
  s32 bodyCount;
  system_body* bodies;
  //u32 rootBodyID;
};

struct galaxy
{
  u32 minSystemGenCount;
  u32 maxSystemGenCount;
  s32 systemCount;
  star_system* systems;
};

struct memory_arena
{
  void* memory;
  memory_index offset;
  u64 size;
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
  
  s32 textLength;
  char textBuffer[GAME_TEXT_BUFFER_MAX_SIZE];
};



#endif //HELIOS_H