/* date = September 15th 2026 0:32 pm */

#ifndef HELIOS_MATH_H
#define HELIOS_MATH_H

#if HELIOS_MSVC
#include "intrin.h"
#endif

#include "math.h"
#include "stdlib.h"


read_only global f32 EulersNumberF32 = 2.7182818284f;
read_only global f64 EulersNumberF64 = 2.7182818284590452353602874713527;
read_only global f32 PiF32 = 3.14159265358979323846f;
read_only global f64 PiF64 = 3.14159265358979323846;

function inline f32 SinF32(f32 x)
{
  f32 result = sinf(x);
  return result;
}

function inline f64 SinF64(f64 x)
{
  f64 result = sin(x);
  return result;
}

function inline f32 CosF32(f32 x)
{
  f32 result = cosf(x);
  return result;
}

function inline f64 CosF64(f64 x)
{
  f64 result = cos(x);
  return result;
}

function inline f64 Atan2F64(f64 x, f64 y)
{
  f64 result = atan2(x, y);
  return result;
}

function inline f64 RadsFromDegreesF64(f64 degrees)
{
  f64 radians = degrees * (PiF64 / 180.0); 
  return radians;
}

function inline f64 DegreesFromRadsF64(f64 radians)
{
  f64 degrees = radians * (180.0/PiF64); 
  return degrees;
}

function inline f32 SqrtF32(f32 x)
{
  return sqrtf(x);
}

function inline f64 SqrtF64(f64 x)
{
  return sqrt(x);
}

function inline f32 PowF32(f32 base, f32 exponent)
{
  f32 result = powf(base, exponent);
  return result;
}

function inline f64 PowF64(f64 base, f64 exponent)
{
  f64 result = pow(base, exponent);
  return result;
}


function inline s32 RoundNegF32ToS32(f32 x)
{
  HS_Assert(x < 0);
  s32 result = (s32)(x - 0.5f);
  return result;
}

function inline s32 RoundPosF32ToS32(f32 x)
{
  HS_Assert(x >=  0);
  s32 result = (s32)(x + 0.5f);
  return result;
}

function inline s32 RoundF32ToS32(f32 x)
{
  s32 result = (s32)((x >= 0) ? (x + 0.5f) : (x - 0.5f));
  return result;
}

function inline s64 RoundNegF64ToS64(f64 x)
{
  HS_Assert(x < 0);
  s64 result = (s64)(x - 0.5f);
  return result;
}

function inline s64 RoundPosF64ToS64(f64 x)
{
  HS_Assert(x >= 0);
  s64 result = (s64)(x + 0.5f);
  return result;
}

function inline s64 RoundF64ToS64(f64 x)
{
  s64 result = (s64)((x > 0) ? (x + 0.5f) : (x - 0.5f));
  return result;
}

function inline u32 RoundF32ToU32(f32 x)
{
  HS_Assert(x >= 0);
  u32 result = (u32)(x + 0.5f);
  return result;
}

function inline s32 AbsS32(s32 n)
{
  s32 result = abs(n);
  return result;
}

function inline f32 AbsF32(f32 n)
{
  f32 result = fabsf(n);
  return result;
}

function inline f64 AbsF64(f64 n)
{
  f64 result = fabs(n);
  return result;
}

function inline s32 ModS32(s32 a, s32 b)
{
  s32 result = a % b;
  return result < 0 ? result + AbsS32(b) : result; 
}

function inline f64 ModF64(f64 a, f64 b)
{
  f64 result = fmod(a, b);
  return result;
};

function inline f32 LerpF32(f32 a, f32 b, f32 t)
{
  f32 result = a + (b - a)*t;
  return result;
}

function inline f64 LerpF64(f64 a, f64 b, f64 t)
{
  f64 result = a;
  if(a != b)
  {
    result = a + (b - a)*t;
  }
  return result;
}


function inline s32 RandS32(s32 min, s32 max)
{
  s32 result = ((rand() % ((max + 1) - min)) + min);
  return result;
}

struct vec2_f32 
{
  f32 x;
  f32 y;
};

function inline vec2_f32
Vec2F32(f32 x, f32 y)
{
  vec2_f32 result = {x, y};
  return result;
}

struct vec2_f64
{
  f64 x;
  f64 y;
};

function inline vec2_f64
Vec2F64(f64 x, f64 y)
{
  vec2_f64 result = {x, y};
  return result;
}

function inline f32 Vec2Magnitude(f32 x, f32 y)
{
  return SqrtF32((x * x) + (y * y));
}


function inline f64 Vec2F64Magnitude(f64 x, f64 y)
{
  return SqrtF64((x * x) + (y * y));
}


function inline vec2_f32 Vec2Normalize(f32 x, f32  y, f32 magnitude=0)
{
  vec2_f32 result = {};
  magnitude = (magnitude) ? magnitude : Vec2Magnitude(x, y);
  result.x = (magnitude) ? x / magnitude : 0;
  result.y = (magnitude) ? y / magnitude : 0;
  return result;
}

#endif //HELIOS_MATH_H
