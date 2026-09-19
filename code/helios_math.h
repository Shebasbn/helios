/* date = September 15th 2026 0:32 pm */

#ifndef HELIOS_MATH_H
#define HELIOS_MATH_H

#if HELIOS_MSVC
#include "intrin.h"
#endif

#include "math.h"

function inline f32 SqrtF32(f32 x)
{
  return sqrtf(x);
}

function inline f64 SqrtF64(f64 x)
{
  return sqrt(x);
}

struct vec2
{
  f32 x;
  f32 y;
};

function inline f32 Vec2Magnitude(f32 x, f32 y)
{
  return SqrtF32((x * x) + (y * y));
}

function inline vec2 Vec2Normalize(f32 x, f32  y, f32 magnitude=0)
{
  vec2 result = {};
  magnitude = (magnitude) ? magnitude : Vec2Magnitude(x, y);
  result.x = x / magnitude;
  result.y = y / magnitude;
  return result;
}

#endif //HELIOS_MATH_H
