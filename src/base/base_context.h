#pragma once 

////////////////////////////////
// NOTE(sebas): Context Cracking

// NOTE(sebas): Compiler detection
#if defined(__clang__)
# define COMPILER_CLANG 1
#elif defined(__GNUC__)
# define COMPILER_GCC 1
#elif defined(_MSC_VER)
# define COMPILER_CL 1
#else
# error no context craking for this compiler
#endif

// NOTE(sebas): OS detection
#if defined(_WIN32)
# define OS_WINDOWS 1
#elif defined(__gnu_linux__)
# define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
# define OS_MAC 1
#else
# error missing OS detection
#endif

// NOTE(sebas): ARCH detection
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

// NOTE(sebas): Zero fill missing macro definitions
#if !defined(COMPILER_CL)
# define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif

#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_MAC)
# define OS_MAC 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif

#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_ARM)
# define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif

///////////////////////////////////////////////////////
// NOTE(Sebas): Helios Build Context

// NOTE(Sebas): INTERNAL SLOW
#if defined(HELIOS_INTERNAL) && defined(HELIOS_SLOW)
# ifndef LIGHT_ASSERT
#   define LIGHT_ASSERT         1
# endif
# ifndef SLOW_ASSERT
#   define SLOW_ASSERT          1
# endif
# ifndef ZERO_INIT
#   define ZERO_INIT            1
# endif
# ifndef DISABLE_EXCEPTIONS
#   define DISABLE_EXCEPTIONS   0 
# endif
  

// NOTE(Sebas): INTERNAL FAST
#elif defined(HELIOS_INTERNAL) && !defined(HELIOS_SLOW)
# ifndef LIGHT_ASSERT
#   define LIGHT_ASSERT         1
# endif
# ifndef SLOW_ASSERT
#   define SLOW_ASSERT          0
# endif
# ifndef ZERO_INIT
#   define ZERO_INIT            0
# endif
# ifndef DISABLE_EXCEPTIONS
#   define DISABLE_EXCEPTIONS   1 
# endif

// NOTE(Sebas): SHIP SLOW
#elif !defined(HELIOS_INTERNAL) && defined(HELIOS_SLOW)
# ifndef LIGHT_ASSERT
#   define LIGHT_ASSERT         1
# endif
# ifndef SLOW_ASSERT
#   define SLOW_ASSERT          1
# endif
# ifndef ZERO_INIT
#   define ZERO_INIT            1
# endif
# ifndef DISABLE_EXCEPTIONS
#   define DISABLE_EXCEPTIONS   0 
# endif

// NOTE(Sebas): SHIP FAST
#else // Handles !HELIOS_INTERNAL && !HELIOS_SLOW
# ifndef LIGHT_ASSERT
#   define LIGHT_ASSERT         0
# endif
# ifndef SLOW_ASSERT
#   define SLOW_ASSERT          0
# endif
# ifndef ZERO_INIT
#   define ZERO_INIT            0
# endif
# ifndef DISABLE_EXCEPTIONS
#   define DISABLE_EXCEPTIONS   1 
# endif
#endif

// NOTE(Sebas): Language
#if defined(__cplusplus)
# define LANG_CXX 1
#else
# define LANG_C   1
#endif

#if !defined(LANG_CXX)
# define LANG_CXX 0
#endif
#if !defined(LANG_C)
# define LANG_C 0
#endif


// NOTE(Sebas): Determine Intrinsics Mode
#if OS_WINDOWS
#if COMPILER_CL
# define INTRINSICS_MICROSOFT 1
#endif
#endif

// NOTE(Sebas): Intrinsic Context
#if !defined(INTRINSICS_MICROSOFT)
# define INTRINSICS_MICROSOFT 0
#endif

#if COMPILER_CL
#define warning_push() pragma warning(push)
#define warning_pop() pragma warning(pop)
#define warnging_disable(w) pragma warning(disable : w)
#define warnging_enable(w) pragma warning(enable : w)
#else

#endif

// NOTE(Sebas):  Warnings & Error Manangement
#if COMPILER_CL && HELIOS_INTERNAL
# pragma warning(disable : 4201)
# pragma warning(disable : 5246)
# pragma warning(disable : 4505)
# pragma warning(disable : 5045)
# pragma warning(disable : 4042)
# pragma warning(disable : 4100)
# if LIGHT_ASSERT || SLOW_ASSERT
# pragma warning(disable : 4804)
#endif
# pragma warning(disable : 4062)
# pragma warning(error : 5246)
#endif
