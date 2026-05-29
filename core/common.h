/* date = May 27th 2026 1:22 pm */

#ifndef COMMON_H
#define COMMON_H

////////////////////////////////////////////////////////////////
//~ Sebas: Context Cracking (Compiler, Language, and OS Detection) 

// NOTE(Sebas): Compiler 
#if defined(_MSC_VER)
# define COMPILER_MSVC 1

# if _MSC_VER >= 1920
#  define COMPILER_MSVC_YEAR 2019
# elif _MSC_VER >= 1910
#  define COMPILER_MSVC_YEAR 2017
# elif _MSC_VER >= 1900
#  define COMPILER_MSVC_YEAR 2015
# elif _MSC_VER >= 1800
#  define COMPILER_MSVC_YEAR 2013
# elif _MSC_VER >= 1700
#  define COMPILER_MSVC_YEAR 2012
# elif _MSC_VER >= 1600
#  define COMPILER_MSVC_YEAR 2010
# elif _MSC_VER >= 1500
#  define COMPILER_MSVC_YEAR 2008
# elif _MSC_VER >= 1400
#  define COMPILER_MSVC_YEAR 2005
# else
#  define COMPILER_MSVC_YEAR 0
# endif

#elif defined(__clang__)
# define COMPILER_CLANG 1
#elif defined(__GNUC__)
# define COMPILER_GCC 1
#else
# error "Can't find current compiler!"
#endif

////////////////////////////////////////////////////////////////
//~ Sebas: Platform 

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
# define PLATFORM_WINDOWS 1
# ifndef _WIN64
#  error "64-bit is required on Windows!"
# endif
#elif defined(__linux__) || defined(__gnu__linux__)
# define PLATFORM_LINUX 1 
# if defined(__ANDROID__)
#  define PLATORM_ANDROID 1
# endif
#elif defined(__unix__)
# define PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
# define PLATFORM_POSIX 1
#elif defined(__APPLE__) 
# define PLATFORM_APPLE 1
# include <TargetConditionals.h>
# if TARGET_IPHONE_SIMULATOR
#  define PLATFORM_IOS 1
#  define PLATFORM_IOSIM 1
# elif TARGET_OIPHONE
#  define PLATFORM_IOS 1
# elif TARGET_OMAC
# else
#  error "Unknown Apple platform"
# endif
#else
# error "Unknown Platform!"
#endif

////////////////////////////////////////////////////////////////
//~ Sebas: Architecture

#if defined(__amd64__) || defined(_M_AMD64)
# define ARCH_X64 1
#elif defined(_M_IX86) || defined(__i386__)
# define ARCH_X86 1
#elif defined(__arm__) || defined(_M_ARM)
# define ARCH_ARM 1
#elif defined(__aarch64__)
# define ARCH_ARM64 1
#else
# error missing ARCH detection
#endif

////////////////////////////////////////////////////////////////
//~ Sebas: Language

#if defined(__cplusplus)
# define LANG_CPP 1
#else
# define LANG_C 1
#endif

////////////////////////////////////////////////////////////////
//~ Sebas: Zero

#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif
#if !defined(ARCH_ARM)
# define ARCH_ARM 0
#endif
#if !defined(COMPILER_MSVC)
# define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif
#if !defined(PLATFORM_WINDOWS)
# define PLATFORM_WINDOWS 0
#endif
#if !defined(PLATFORM_LINUX)
# define PLATFORM_LINUX 0
#endif
#if !defined(PLATFORM_ANDROID)
# define PLATFORM_ANDROID 0
#endif
#if !defined(PLATFORM_UNIX)
# define PLATFORM_UNIX 0
#endif
#if !defined(PLATFORM_POSIX)
# define PLATFORM_POSIX 0
#endif
#if !defined(PLATFORM_APPLE)
# define PLATFORM_APPLE 0
#endif
#if !defined(PLATFORM_IOS)
# define PLATFORM_IOS 0
#endif
#if !defined(PLATFORM_IOSIM)
# define PLATFORM_IOSIM 0
#endif
#if !defined(LANG_CPP)
# define LANG_CPP 0
#endif
#if !defined(LANG_C)
# define LANG_C 0
#endif


////////////////////////////////////////////////////////////////
//~ Sebas: Primative Types

#include <stdint.h>
//#include <stddef.h>

typedef int8_t   S8; 
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;

typedef float    F32;
typedef double   F64;

typedef void VoidFunction(void);

////////////////////////////////////////////////////////////////
//~ Sebas: Codebase Keywords

#define internal static
#define global static
#define local static
#define fallthrough

#if COMPILER_MSVC
# define thread_var __declspec(thread)
#else
# define thread_var __thread
#endif

#if COMPILER_MSVC
#define force_inline __forceinline
#elif COMPILER_CLANG || COMPILER_GCC
#define force_inline inline __attribute__((always_inline))
#else
#define force_inline inline
#endif

#if COMPILER_MSVC && COMPILER_MSVC_YEAR < 2015
# define this_functon_name "unkown"
#else
# define this_function_name __func__
#endif

#if COMPILER_MSVC || (COMPILER_CLANG && PLATFORM_WINDOWS)
# pragma section(".rdata$", read)
# define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && PLATFORM_LINUX)
# define read_only __attribute__((section(".rodata")))
#else
# define read_only 
#endif


////////////////////////////////////////////////////////////////
//~ Sebas: Global Macros & Utilites

#define Stmnt(stmnt) do { stmnt } while(0)
#define UseVar(name) ((void)(name))
#define Swap(type, a, b) Stmnt( type _swapper_ = a; a = b; b = _swapper_; )

#if COMPILER_MSVC
# define AlignOf() __alignof(T)
#elif COMPILER_CLANG
# define AlignOf() __alignof(T)
#elif COMPILER_GCC
# define AlignOf() __alignof__(T)
#else
# error AlignOf not defined for this compiler.
#endif

////////////////////////////////////////////////////////////////
//~ Sebas: Arrays/Pointers/Integers

#define ArrayCount(arr) (sizeof(arr) / sizeof(*(arr)))
#define IntFromPtr(ptr) (U64)(((U8*)ptr) - 0)
#define PtrFromInt(n) (void*)(((U8*)0) + n)
#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define Member(type, member_name) ((type *)0)->member_name
#define OffsetOf(type, member_name) IntFromPtr(&Member(type, member_name))
#define BaseFromMember(type, member_name, ptr) (type *)((U8 *)(ptr) - OffsetOf(type, member_name))

////////////////////////////////////////////////////////////////
//~ Sebas: Units

#define Bytes(n) (n)
#define Kilobytes(n) (n << 10)
#define Megabytes(n) (n << 20)
#define Gigabytes(n) (((U64)n) << 30)
#define Terabytes(n) (((U64)n) << 40)

#define Thousand(n) ((n)*1000)
#define Million(n)  ((n)*1000000)
#define Billion(n)  ((n)*1000000000LL)

////////////////////////////////////////////////////////////////
//~ Sebas: Asserts

#ifndef ASSERTS_ENABLED 
# define ASSERTS_ENABLED 1
#endif

# if PLATFORM_WINDOWS && COMPILER_MSVC
#  define _DebugBreak() __debugbreak()
# elif COMPILER_GCC 
#  define _DebugBreak() __builtin_trap()
# else
#  define _DebugBreak() (*(volatile int*)0 = 0)
# endif

#define AssertBody(c)  Stmnt( if (!(c)) { _DebugBreak(); } ) 

#define Verify(c) AssertBody((c))

# define StaticBreak_(c, label) static_assert(c, label)

// TODO(Sebas): Define when StaticAsserts are enabled!
# undef StaticAssert
# if 1
#  define StaticAssert(c, label) StaticBreak_(c, label)
# else
#  define StaticAssert(c, label) 
# endif

#undef Assert
#if ASSERTS_ENABLED
# define Assert(c) AssertBody((c))
#else
# define Assert(c) 
#endif


////////////////////////////////////////////////////////////////
//~ Sebas: Intrinsics

#include "intrinsics.h"

#define M_MemorySet(ptr, val, size) _mem_set((ptr), (val), (size))
#define M_MemoryCopy(dest, src, size) _mem_copy((dest), (src), (size))
#define M_MemoryMove(dest, src, size) _mem_move((dest), (src), (size))

////////////////////////////////////////////////////////////////
//~ Sebas: Memory Macros

#define M_ZeroMemory(ptr, size) M_MemorySet((ptr),0,(size))
#define M_ZeroStruct(ptr) M_ZeroMemory((ptr), sizeof(*(ptr)))
#define M_ZeroArray(arr) M_ZeroMemory((arr), sizeof(arr))
//#define ZeroCount(ptr, count) ZeroMemory((ptr), sizeof(*(ptr))*(count))

#define M_CopyMemory(dest, src, size) M_MemoryCopy((dest), (src), (size))
#define M_CopyStruct(dest, src) Stmnt(Assert(sizeof(*(dest)) == sizeof(*(src))); M_MemoryCopy((dest),(src), sizeof(*(src)));)
#define M_CopyArray(dest, src) Stmnt(Assert(sizeof((dest)) == sizeof((src))); M_MemoryCopy((dest),(src), sizeof((src)));)
//#define CopyCount(dest, src, count) MemoryCopy((dest), (src), sizeof(*(dest)) * (count))

////////////////////////////////////////////////////////////////
//~ Sebas: Linked List Operations



#define CheckNull(p) ((p)==0)
#define SetNull(p) ((p)=0)

//#define QueuePush_NZ

//#define Quee

////////////////////////////////////////////////////////////////
//~ Sebas: Min, Max, Clamps 

#define Min(a, b) (((a)<(b)) ? (a) : (b))
#define Max(a, b) (((a)>(b)) ? (a) : (b))
#define ClampTop(x, a) Min(x,a)
#define ClampBot(a, x) Max(a,x)
#define Clamp(a, x, b) (((a)>(x))?(a):((b)<(x))?(b):(x))

////////////////////////////////////////////////////////////////
//~ Sebas: Basic Types

struct String8;
struct String16;
struct String32;



struct Arena;
struct ArenaTemp;
////////////////////////////////////////////////////////////////
//~ Sebas: Limits / Constants 



read_only global U8 U8Max = 0xFF;
read_only global U8 U8Min = 0;

read_only global U16 U16Max = 0xFFFF;
read_only global U16 U16Min = 0;

read_only global U32 U32Max = 0xFFFFFFFF;
read_only global U32 U32Min = 0;

read_only global U64 U64Max = 0xFFFFFFFFFFFFFFFF;
read_only global U64 U64Min = 0;

read_only global S8 S8Max = 0x7F;
read_only global S8 S8Min = -1 - 0x7F;

read_only global S16 S16Max = 0x7FFF;
read_only global S16 S16Min = -1 - 0x7FFF;

read_only global S32 S32Max = 0x7FFFFFFF;
read_only global S32 S32Min = -1 - 0x7FFFFFFF;

read_only global S64 S64Max = 0x7FFFFFFFFFFFFFFF;
read_only global S64 S64Min = -1 - 0x7FFFFFFFFFFFFFFF;

read_only global U32 SignF32 = 0x80000000;
read_only global U32 ExponentF32 = 0x7F800000;
read_only global U32 MantissaF32 = 0x7FFFFF;

read_only global F32 F32Max = 3.402823e+38f;
read_only global F32 F32Min = -3.402823e+38f;
read_only global F32 F32SmallestPositive = 1.1754943508e-38f;
read_only global F32 F32Epsilon = 5.96046448e-8f;

read_only global U64 SignF64 = 0x8000000000000000ull;
read_only global U64 ExponentF64 = 0x7FF0000000000000ull;
read_only global U64 MantissaF64 = 0xFFFFFFFFFFFFFull;

////////////////////////////////////////////////////////////////
//~ Sebas: Nill Values

read_only global S8  S8_NILL_VALUE = 0;
read_only global S16 S16_NILL_VALUE = 0;
read_only global S32 S32_NILL_VALUE = 0;
read_only global S64 S64_NILL_VALUE = 0;
read_only global U8  U8_NILL_VALUE = 0;
read_only global U16 U16_NILL_VALUE = 0;
read_only global U32 U32_NILL_VALUE = 0;
read_only global U64 U64_NILL_VALUE = 0;
read_only global F32 F32_NILL_VALUE = 0;
read_only global F64 F64_NILL_VALUE = 0;
read_only global B8  B8_NILL_VALUE = 0;
read_only global B16 B16_NILL_VALUE = 0;
read_only global B32 B32_NILL_VALUE = 0;
read_only global B64 B64_NILL_VALUE = 0;

force_inline const S8*  S8_NILL_PTR()  { return &S8_NILL_VALUE; }
force_inline const S16* S16_NILL_PTR() { return &S16_NILL_VALUE; }
force_inline const S32* S32_NILL_PTR() { return &S32_NILL_VALUE; }
force_inline const S64* S64_NILL_PTR() { return &S64_NILL_VALUE; }
force_inline const U8*  U8_NILL_PTR()  { return &U8_NILL_VALUE; }
force_inline const U16* U16_NILL_PTR() { return &U16_NILL_VALUE; }
force_inline const U32* U32_NILL_PTR() { return &U32_NILL_VALUE; }
force_inline const U64* U64_NILL_PTR() { return &U64_NILL_VALUE; }
force_inline const F32* F32_NILL_PTR() { return &F32_NILL_VALUE; }
force_inline const F64* F64_NILL_PTR() { return &F64_NILL_VALUE; }
force_inline const B8*  B8_NILL_PTR()  { return &B8_NILL_VALUE; }
force_inline const B16* B16_NILL_PTR() { return &B16_NILL_VALUE; }
force_inline const B32* B32_NILL_PTR() { return &B32_NILL_VALUE; }
force_inline const B64* B64_NILL_PTR() { return &B64_NILL_VALUE; }

#endif //COMMON_H


