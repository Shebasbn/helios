/* date = October 1st 2026 11:24 am */

#ifndef HELIOS_BODY_H
#define HELIOS_BODY_H

enum class system_body_type
{
  Nil = 0,
  Star,
  Planet,
  Moon,
  AsteroidBelt,
};

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
  b32 hasBeenUpdated;
};

// TODO(Sebas): Move all system code into its own header file.
#define MAX_SYSTEM_BODY_COUNT 50
#define NilID (u32)system_body_type::Nil
struct star_system
{
  u32 systemID;
  s32 bodyCount;
  system_body* bodies;
  //u32 rootBodyID;
};

read_only global f64 GravitationalConstant = 6.6743e-11;
read_only global f32 MetersPerHU;

function inline system_body* GetSystemBodyFromID(star_system* system, u32 bodyID)
{
  system_body* result = 0;
  if(bodyID < system->bodyCount)
  {
    result = &system->bodies[bodyID];
  }
  return result;
}

function inline f64 CalculateBodyMeanMotion(f64 massParent, f64 massChild, f64 semiMajor)
{
  f64 result = SqrtF64((GravitationalConstant * (massParent + massChild)) / 
                       (semiMajor * semiMajor * semiMajor));
  return result;
}

function inline f64
CalculateBodySemiMinorAxis(f64 semiMajorAxis, f64 eccentricity)
{
  f64 semiMinorAxis = (semiMajorAxis * SqrtF64(1.0 - (eccentricity * eccentricity)));
  return semiMinorAxis;
}


function f64 SolveKeplersEquations(f64 meanAnomaly, f64 eccentricity);
function void UpdateBodyOrbit(keplerian_body* body, f64 deltaTime);

function void GenerateSolarSystem(memory_arena* arena, star_system* systems, u32 systemID, f32 metersPerHU);



#include "helios_body.cpp"

#endif //HELIOS_BODY_H
