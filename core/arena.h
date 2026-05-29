/* date = May 28th 2026 9:00 am */

#ifndef ARENA_H
#define ARENA_H

#include <common.h>
#include <core_string.h>

////////////////////////////////////////////////////////////////
//~ Sebas: Constants

#define ARENA_HEADER_SIZE 128


//enum ArenaFlags : U8
//{
//ARENA_FLAG_NO_CHAIN = (1 << 0),
//};

struct ArenaParams
{
    //ArenaFlags flags;
    U64 reserve_size;
    U64 commit_size;
    void* opt_back_buffer;
    String8 name;
};

struct Arena
{
    U64 base_pos;
    U64 pos;
    U64 commit_size;
    U64 commited;
    U64 reserved;
    U32 name_size;
};
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, "Aren header size is to big!");

struct ArenaTemp
{
    Arena* arena;
    U64 pos;
};

global U64 arena_default_reserve_size = Megabytes(64);
global U64 arena_default_commit_size  = Kilobytes(64);

force_inline ArenaParams
Params(String8 name, void* bbf=0, U64 rs=arena_default_reserve_size , U64 cs=arena_default_commit_size) 
{
    ArenaParams result = {rs, cs,bbf, name}; 
    return result; 
}

Arena* ArenaAlloc_(ArenaParams params);
#define ArenaAlloc(...) ArenaAlloc_(Params(__VA_ARGS__))

void ArenaRelease(Arena* arena);

void* ArenaPush(Arena* arena, U64 size, U64 align);
U64 ArenaPos(Arena* arena);
void ArenaPopTo(Arena* arena, U64 pos);

void ArenaClear(Arena* arena);
void ArenaPop(Arena* arena, U64 amt);

ArenaTemp TempBegin(Arena* arena);
void TempEnd(ArenaTemp temp);

#define PushArrayNoZeroAligned(a, T, c, align) (T *)ArenaPush((a), sizeof(T)*(c), (align))
#define PushArrayAligned(a, T, c, align) (T *)M_ZeroMemory(PushArrayNoZeroAligned(a, T, c, align), sizeof(T)*(c))
#define PushArrayNoZero(a, T, c) PushArrayNoZeroAligned(a, T, c, Max(8, AlignOf(T)))
#define PushArray(a, T, c) PushArrayAligned(a, T, c, Max(8, AlignOf(T)))

#endif //ARENA_H
