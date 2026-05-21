#pragma once

//////////////////////////////////////////////////
// NOTE(sebas): Memory Types

struct M_Memory
{
    void* ptr;
    U64 size;
};

//////////////////////////////////////////////////
// NOTE(sebas): Arena Types

struct M_Arena
{
    U8* base;
    U64 pos;
    U64 cap;
};

struct M_ArenaTemp
{
    M_Arena* arena;
    U64 pos;
};

struct M_Pool
{
    U8* base;
    U64 pos;
    U64 cap;
    M_Pool* next;
};

#define M_SCRATCH_POOL_COUNT 2

//////////////////////////////////////////////////
// NOTE(sebas): Arena Helper Macros
#define M_ARENA_DEFAULT_ALIGNMENT 8
#define PushSize_(a,s,al) m_arena_push(a, s, al)
#define PushSize(a,s) m_arena_push(a, s)
#define PushAligned(a,s,al) m_arena_push(a, s, al)
#define PushStruct(a,T) (T*)m_arena_push(a, sizeof(T), alignof(T))
#define PushArray(a,T,c) (T*)m_arena_push(a, sizeof(T) * (c), alignof(T))
#define ArenaSpace(a) ((a)->cap - (a)->pos)
#define GetScratch(conflict_arr, count) m_get_scratch((conflict_arr), (count));
#define GetScratchScope(...) GetScratch(((M_Arena*[]){__VA_ARGS__}),\
                                        (sizeof((M_Arena*[]){__VA_ARGS__}) / sizeof(M_Arena*)))
//////////////////////////////////////////////////
// NOTE(sebas): Arena Functions
function M_Arena        m_arena_init(void* memory, U64 size);
function void*          m_arena_push(M_Arena* arena, U64 size, U64 align=M_ARENA_DEFAULT_ALIGNMENT);
function void           m_arena_pop_to(M_Arena* arena, U64 pos);
function void           m_arena_pop_amount(M_Arena* arena, U64 amount);
function void           m_arena_reset(M_Arena* arena);

function M_ArenaTemp    m_begin_temp(M_Arena* arena);
function void           m_end_temp(M_ArenaTemp temp);

function void           m_init_scratch(M_Memory* memory);
function M_ArenaTemp    m_get_scratch(M_Arena** conflict_array, U32 count);
#define                 m_release_scratch(temp) m_end_temp(temp)


