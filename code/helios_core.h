/* date = September 12th 2026 4:20 pm */

#ifndef HELIOS_CORE_H
#define HELIOS_CORE_H

#if defined(_MSC_VER)
# define HELIOS_MSVC 1

# if _MSC_VER >= 1920
#  define HELIOS_MSVC_YEAR 2019
# elif _MSC_VER >= 1910
#  define HELIOS_MSVC_YEAR 2017
# elif _MSC_VER >= 1900
#  define HELIOS_MSVC_YEAR 2015
# elif _MSC_VER >= 1800
#  define HELIOS_MSVC_YEAR 2013
# elif _MSC_VER >= 1700
#  define HELIOS_MSVC_YEAR 2012
# elif _MSC_VER >= 1600
#  define HELIOS_MSVC_YEAR 2010
# elif _MSC_VER >= 1500
#  define HELIOS_MSVC_YEAR 2008
# elif _MSC_VER >= 1400
#  define HELIOS_MSVC_YEAR 2005
# else
#  define HELIOS_MSVC_YEAR 0
# endif
#elif defined(__clang__)
# define HELIOS_CLANG 1
#elif defined(__GNUC__)
# define HELIOS_GCC 1
#else
# error "Can't find current compiler!"
#endif

//~ Sebas: Platform Detection 

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
# define HELIOS_WINDOWS 1
# ifndef _WIN64
#  error "64-bit is required on Windows!"
# endif
#elif defined(__linux__) || defined(__gnu_linux__)
# define HELIOS_LINUX 1
# if defined(__ANDROID__)
#  define HELIOS_ANDROID 1
# endif
#else
# error "Platform not supported."
#endif

////////////////////////////////////////////////////////////////
//~ Sebas: Architecture Detection
//~ TODO(Sebas): Check Arm64 detection, I think, its wrong.

#if defined(__amd64__) || defined(_M_AMD64)
# define HELIOS_X64 1
#elif defined(_M_IX86) || defined(__i386__)
# define HELIOS_X86 1
#elif defined(__arm__) || defined(_M_ARM)
# define HELIOS_ARM 1
#elif defined(__aarch64__)
# define HELIOS_ARM64 1
#else
# error missing HELIOS detection
#endif

//~ Sebas: Language Dectection

#if defined(__cplusplus)
# define HELIOS_CPP 1
#else
# define HELIOS_C 1
#endif

//~ Sebas: Zero Definitions

#if !defined(HELIOS_X64)
# define HELIOS_X64 0
#endif
#if !defined(HELIOS_X86)
# define HELIOS_X86 0
#endif
#if !defined(HELIOS_ARM64)
# define HELIOS_ARM64 0
#endif
#if !defined(HELIOS_ARM)
# define HELIOS_ARM 0
#endif
#if !defined(HELIOS_MSVC)
# define HELIOS_MSVC 0
#endif
#if !defined(HELIOS_GCC)
# define HELIOS_GCC 0
#endif
#if !defined(HELIOS_CLANG)
# define HELIOS_CLANG 0
#endif
#if !defined(HELIOS_WINDOWS)
# define HELIOS_WINDOWS 0
#endif
#if !defined(HELIOS_LINUX)
# define HELIOS_LINUX 0
#endif
#if !defined(HELIOS_ANDROID)
# define HELIOS_ANDROID 0
#endif
#if !defined(HELIOS_CPP)
# define HELIOS_CPP 0
#endif
#if !defined(HELIOS_C)
# define HELIOS_C 0
#endif

#if !defined(HELIOS_RELEASE)
# define HELIOS_RELEASE 0
#endif
#if !defined(HELIOS_DEBUG)
# define HELIOS_DEBUG !HELIOS_RELEASE
#endif

#if HELIOS_RELEASE == HELIOS_DEBUG
# define HELIOS_DEBUG !HELIOS_RELEASE
#endif

////////////////////////////////////////////////////////////////
//~ Sebas: Codebase Keywords

#define function static 
#define global static 
#define local_persist static 
#define fallthrough

#if HELIOS_CPP
# define no_name_mangle extern "C"
#else
# define no_name_mangle
#endif

#if HELIOS_MSVC || (HELIOS_CLANG && HELIOS_WINDOWS)
# pragma section(".rdata$", read)
# define read_only __declspec(allocate(".rdata$"))
#elif (HELIOS_CLANG && HELIOS_LINUX)
# define read_only __attribute__((section(".rodata")))
#else
# define read_only
#endif


////////////////////////////////////////////////////////////////
//~ Sebas: Basic Types

#include <stdint.h>
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef s8       b8;
typedef s16      b16;
typedef s32      b32;
typedef s64      b64;
typedef float    f32;
typedef double   f64;

////////////////////////////////////////////////////////////////
//~ Sebas: Helper Macros

#define HS_Statement(stmnt) do{ stmnt }while(0)

#if HELIOS_WINDOWS
# define HS_DebugBreak() __debugbreak()
#else
# define HS_DebugBreak() (*(volatile int *)0 = 0)
#endif

#define HS_AssertAlways(condtion) HS_Statement(if(!(condition)){HS_DebugBreak();})  

#if HELIOS_DEBUG 
# define HS_Assert(condition) HS_Statement(if(!(condition)){HS_DebugBreak();})  
#else
# define HS_Assert(condition)
#endif


#define HS_Stringify_(x) #x
#define HS_Stringify(x) HS_Stringify_(x)



#define HS_ArrayCount(array) sizeof(array) / sizeof(array[0])

#define HS_Min(a, b) (((a)<(b)) ? (a) : (b))
#define HS_Max(a, b) (((a)>(b)) ? (a) : (b))
#define HS_ClampTop(x, a) HS_Min(x, a)
#define HS_ClampBot(a, x) HS_Max(a, x)
#define HS_Clamp(a, x, b) (((a)>(x))?(a):((b)<(x))?(b):(x))

////////////////////////////////////////////////////////////////
//~ Sebas: Units 

#define HS_Bytes(n)      (n)
#define HS_Kilobytes(n)  (n << 10)
#define HS_Megabytes(n)  (n << 20)
#define HS_Gigabytes(n)  (((u64)n) << 30)
#define HS_Terabytes(n)  (((u64)n) << 40)

#define HS_Thousand(n) ((n)*1000)
#define HS_Million(n)  ((n)*1000000)
#define HS_Billion(n)  ((n)*1000000000LL)

read_only global s32 MSPerSecond    = 1000;
read_only global f64 SecondsPerMS   = 1e-3;
read_only global s32 TicksPerMS     = 10000;
read_only global f64 MSPerTick      = 1e-4;
read_only global s32 TicksPerSecond = 10000000;
read_only global f64 SecondsPerTick = 1e-7;

read_only global u8 U8Max = 0xFF;
read_only global u8 U8Min = 0;

read_only global u16 U16Max = 0xFFFF;
read_only global u16 U16Min = 0;

read_only global u32 U32Max = 0xFFFFFFFF;
read_only global u32 U32Min = 0;

read_only global u64 U64Max = 0xFFFFFFFFFFFFFFFF;
read_only global u64 U64Min = 0;

read_only global s8 S8Max = 0x7F;
read_only global s8 S8Min = -1 - 0x7F;

read_only global s16 S16Max = 0x7FFF;
read_only global s16 S16Min = -1 - 0x7FFF;

read_only global s32 S32Max = 0x7FFFFFFF;
read_only global s32 S32Min = -1 - 0x7FFFFFFF;

read_only global s64 S64Max = 0x7FFFFFFFFFFFFFFF;
read_only global s64 S64Min = -1 - 0x7FFFFFFFFFFFFFFF;



////////////////////////////////////////////////////////////////
//~ Sebas: Helper Functions

function inline s32
S32FromZ(char* at)
{
  s32 result = 0;
  while((*at >= '0') &&
        (*at <= '9'))
  {
    result *= 10;
    result += (*at - '0');
    ++at;
  }
  return result;
}


function inline u32 
SafeTruncateU64(u64 value)
{
  //~ TODO(Sebas):  Defines for maximum values
  HS_Assert(value <= 0xFFFFFFFF);
  u32 result = (u32)value;
  return result;
}



#endif //HELIOS_CORE_H
