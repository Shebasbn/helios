/* date = May 27th 2026 1:45 pm */

#ifndef INTRINSICS_H
#define INTRINSICS_H

////////////////////////////////////////////////////////////////
//~ Sebas: Memory Intrinsics 

#if COMPILER_MSVC
extern "C" void* __cdecl memset(void*, int, size_t);
extern "C" void* __cdecl memcpy(void*, const void*, size_t);
extern "C" void* __cdecl memmove(void*, const void*, size_t);
# pragma intrinsic(memset, memcpy, memmove)
# define _mem_set(ptr, val, size) memset((ptr), (val), (size))
# define _mem_copy(dest, src, size) memcpy((dest), (src), (size))
# define _mem_move(dest, src, size) memmove((dest), (src), (size))



#elif COMPILER_CLANG || COMPILER_GCC
# define _mem_set(ptr, val, size) __builtin_memset((ptr), (val), (size))
# define _mem_copy(dest, src, size) __builtin_memcpy((dest), (src), (size))
# define _mem_move(dest, src, size) __builtin_memmove((dest), (src), (size))
#endif

#if COMPILER_MSVC
extern "C" void  __cdecl _ReadWriteBarrier(void);
extern "C" void  __cdecl _WriteBarrier(void);
extern "C" void  __cdecl _ReadBarrier(void);
extern "C" void __faststorefence(void);
# if ARCH_X64 || ARCH_X86
#  define ReadWriteMemoryBarrier() _ReadWriteBarrier()
#  define WriteMemoryBarrier() _WriteBarrier()
#  define ReadMemoryBarrier() _ReadBarrier()
# elif ARCH_ARM || ARCH_ARM64
#  include <arm64_intrinsics.h>
#  define ReadWriteMemoryBarrier() __dmb(_ARM64_BARRIER_ISH)
#  define WriteMemoryBarrier() __dmb(_ARM64_BARRIER_ISHST)
#  define ReadMemoryBarrier() __dmb(_ARM64_BARRIER_ISHLD)
# else
#  error Unsupported Platform!
# endif
#elif COMPILER_CLANG || COMPILER_GCC
# if ARCH_X64 || ARCH_X86
#  define ReadWriteMemoryBarrier() __asm__ __volatile__("" ::: "memory")
#  define WriteMemoryBarrier() __asm__ __volatile__("" ::: "memory")
#  define ReadMemoryBarrier() __asm__ __volatile__("" ::: "memory")
# elif ARCH_ARM || ARCH_ARM64
#  define ReadWriteMemoryBarrier() __asm__ __volatile__("dmb ish"   ::: "memory")
#  define WriteMemoryBarrier() __asm__ __volatile__("dmb ishst" ::: "memory")
#  define ReadMemoryBarrier()  __asm__ __volatile__("dmb ishld" ::: "memory")
# else
#  error Unsupported Platform!
# endif
#endif

#endif //INTRINSICS_H
