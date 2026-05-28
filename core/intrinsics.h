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

#endif //INTRINSICS_H
