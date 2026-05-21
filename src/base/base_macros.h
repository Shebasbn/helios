#pragma once

////////////////////////////////
// NOTE(sebas): Helper Macros
#define Stmnt(S) do { S }while (0)

#define Stringify_(S) #S
#define StringifyW_(S) L#S
#define Stringify(S) Stringify_(S)
#define StringifyW(S) StringifyW_(S)
#define Glue_(A, B) A##B
#define Glue(A, B) Glue_(A, B)

#ifdef intrinsic_debug_break
# define AssertBreak() intrinsic_debug_break()
#else
# define AssertBreak() (*(S32*)0 = 0)
#endif

// TODO(Sebas): Do the Checking to see if work else where
#if LANG_CXX
#define StaticMessage(c) Stringify(Glue((c) = , Glue( > Line, (__LINE__))))
#define StaticAssert_(c, msg) static_assert(c, msg)
#else
#define StaticAssert_(c, l) typedef U8 Glue(c, __LINE__) [(c)?1:-1]
#endif



#if LIGHT_ASSERT || SLOW_ASSERT
#define StaticAssertMsg(c, msg) StaticAssert_(c,msg)
#define StaticAssert(c) StaticAssert_(c, StaticMessage(c))
#define Assert_(c) Stmnt(if (!(c)) { AssertBreak(); })
#else
# define StaticAssertMsg(c, msg)
# define StaticAssert(c)
#endif

#if LIGHT_ASSERT
#  define LightAssert(c) Assert_(c)
#elif SLOW_ASSERT
#  define SlowAssert(c) Assert_(c)
#  define LightAssert(c) Assert_(c)
#else
# define LightAssert(c)
# define SlowAssert(c)
#endif

#ifdef HELIOS_LOG

#else
#define HeliosLog(msg) printf("LOG: %s\n", msg);
#endif

#define ArrayCount(a) (sizeof(a) / sizeof(*(a)))

#define IntFromPtr(p) (U64)((U8*)p - (U8*)0)
#define PtrFromInt(n) (void*)((U8*)0 + (n))

#define Member(T, m) (((T *)0)->m)
#define OffsetOfMember(T, m) IntFromPtr(&Member(T, m))

#define Implies(a,b) (!(a) || (b))

#define Min(a, b) (((a) < (b)) ? (a):(b))
#define Max(a, b) (((a) > (b)) ? (a):(b))
#define Clamp(a, x, b) (((x) < (a)) ? (a):\
                        ((b) < (x)) ? (b):(x))

#define ClampTop(a, b) Min(a, b)
#define ClampBot(a, b) Max(a, b)


#define AlignUpPow2(x, p) (((x) + (p) - 1)&~((p)-1))
#define AlignDownPow2(x, p) ((x) & ~((p) - 1))
#define GetPpdPow2(x, p) (((x) - ((x)%(p)))%(p))
#define IsPow2(x, p) (((x)&((x)-1)) == 0)

#define Bool(b) ((b) != 0)

#define KB(n) ((n) << 10)
#define MB(n) ((n) << 20)
#define GB(n) ((n) << 30)
#define TB(n) ((U64)(n) << 40llu)

#define Thousand(x) ((x)*1000)
#define Million(x)  ((x)*1000000llu)
#define Billion(x)  ((x)*1000000000llu)
#define Trillion(x) ((x)*1000000000000llu)

#if LANG_CXX
# define c_linkage extern "C"
# define read_only constexpr
#else
# define c_linkage extern
# define read_only const
#endif

#if COMPILER_CL
# define threadvar __declspec(thread)
#elif COMPILER_CLANG
# define threadvar __thread
#else
# error threadvar not defined for this compiler
#endif

#if COMPILER_GCC || COMPILER_CLANG
# define force_inline inline __attribute__((always_inline))
#elif COMPILER_CL
# define force_inline __forceinline
#else
# define force_inline inline
#endif

#if COMPILER_CL
# define shared_export __declspec(dllexport)
#elif COMPILER_CLANG
# define shared_export __declspec(dllexport)
#else
# error shared_export not defined for this compiler
#endif

#define link_global c_linkage
#define link_threadvar c_linkage threadvar
#define global static
#define local static
#define function static
#define link_function c_linkage
#define shared_function c_linkage shared_export


#include <string.h>
#define MemoryZero(p,z)      memset((p), 0, (z))
#define MemoryZeroStruct(p)  MemoryZero((p), sizeof(*(p)))
#define MemoryZeroArray(p)   MemoryZero((p), sizeof(p))
#define MemoryZeroTyped(p,c) MemoryZero((p), sizeof(*(p))*(c))

#define MemoryMatch(a,b,z) (memcmp((a),(b),(z)) == 0)

#define MemoryCopy(d,s,z) memmove((d), (s), (z))
#define MemoryCopyStruct(d,s) MemoryCopy((d),(s),Min(sizeof(*(d)),sizeof(*(s))))
#define MemoryCopyArray(d,s)  MemoryCopy((d),(s),Min(sizeof(s),sizeof(d)))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),Min(sizeof(*(d)),sizeof(*(s)))*(c))

////////////////////////////////
// NOTE(sebas): Linked List Macros

#define DLLPushBack_NP(f,l,n,next,prev) ((f)==0?\
                                            (f)=(l)=(n), (n)->next=(n)->prev=0:\
                                            ((l)->next=(n), (n)->prev=(l), (l)=(n), (l)->next=0))

#define DLLPushBack(f,l,n) DLLPushBack_NP(f,l,n,next,prev)
#define DLLPushFront(f,l,n) DLLPushBack_NP(l,f,n,prev,next)
#define DLLRemove_NP(f,l,n,next,prev) (((f)==n?\
                                        ((f)=(f)->next, (f)->prev=0):\
                                         (l)==(n)?\
                                            ((l)=(l)->prev, (l)->next=0):\
                                            ((n)->next->prev=(n)->prev,\
                                             (n)->prev->next=(n)->next)))

#define DLLRemove(f,l,n) DLLRemove_NP(f,l,n,next,prev)

#define SLLQueuePush_N(f,l,n,next) ((f)==0?\
                                    (f)=(l)=(n):\
                                    ((l)->next=(n), (l)=(n)), (n)->next=0)
#define SLLQueuePush(f,l,n) SLLQueuePush_N(f,l,n,next)

#define SLLQueuePushFront_N(f,l,n,next) ((f)==0?\
                                        ((f)=(l)=(n), (n)->next=0):\
                                        ((n)->next=(f), (f)=(n)))
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_N(f,l,n,next)
#define SLLQueuePop_N(f,l,next) ((f)==0?\
                                 (f)=(l)=0:\
                                 (f)=(f)->next)

#define SLLQueuePop(f,l) SLLQueuePop_N(f,l,next)

#define SLLStackPush_N(f,n,next) ((f)==0?\
                                  (f)=(n), (f)->next=0:\
                                 ((n)->next=(f), (f)=(n)))
#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)
#define SLLStackPop_N(f,next) ((f)==0?0:\
                               (f)=(f)->next)

#define SLLStackPop(f) SLLStackPop_N(f,next)

