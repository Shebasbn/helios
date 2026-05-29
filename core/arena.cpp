#include <arena.h>
#include <platform.h>
#include <core_string.h>

Arena* ArenaAlloc_(ArenaParams params)
{
    U64 reserve_size = params.reserve_size;
    U64 commit_size = params.commit_size;
    reserve_size = AlignPow2(reserve_size, PF_AllocGranularity());
    commit_size = AlignPow2(commit_size, PF_PageSize());
    
    void* base = params.opt_back_buffer;
    if (base == 0)
    {
        void* arena = PF_MemoryReserve(reserve_size);
        PF_MemoryCommit(base, commit_size);
    }
    
    Arena* arena = (Arena*)base;
    /*arena->flags = params->flags;*/
    arena->name_size = params.name.size;
    // TODO(Sebas): Clamp 64 -> 32 bit
    arena->commit_size = params.commit_size;
    //arena->reserve_size = params->reserve_size;
    arena->base_pos = 0;
    arena->pos = ARENA_HEADER_SIZE;
    arena->commited = commit_size;
    arena->reserved = reserve_size;;
    
    if (arena->name_size != 0)
    {
        U8* name_ptr = (U8*)ArenaPush(arena, arena->name_size, 1);
        M_CopyMemory(name_ptr, params.name.str, params.name.size);
        ArenaPush(arena, 0, 128);
    }
    
    /*arena->current = arena;*/
    
    return arena;
}

void ArenaRelease(Arena* arena)
{
    PF_MemoryRelease(arena);
}

void* ArenaPush(Arena* arena, U64 size, U64 align)
{
    U64 pos_pre = AlignPow2(arena->pos, align);
    U64 pos_post = pos_pre + size;
    
    if (arena->commited < pos_post)
    {
        U64 target_aligned = AlignPow2(pos_post, arena->commit_size);
        target_aligned = AlignPow2(target_aligned, PF_PageSize());
        U64 cmt_clamped =  ClampTop(target_aligned, arena->reserved);
        U64 clamped_cmt_size = cmt_clamped - arena->commited;
        U8* cmt_ptr = (U8*)arena + arena->commited;
        PF_MemoryCommit(cmt_ptr, clamped_cmt_size);
        arena->commited = cmt_clamped;
    }
    
    void* result = 0;
    if (arena->commited >= pos_post)
    {
        result = (U8*)arena + pos_pre;
        arena->pos = pos_post;
    }
    return result;
}

U64 ArenaPos(Arena* arena)
{
    U64 pos = arena->base_pos + arena->pos;
    return pos;
}
void ArenaPopTo(Arena* arena, U64 pos)
{
    U64 new_pos = ClampBot(ARENA_HEADER_SIZE, pos) - arena->base_pos;
    Verify(new_pos <= arena->pos);
    arena->pos = new_pos;
}

void ArenaClear(Arena* arena)
{
    ArenaPopTo(arena, 0);
}

void ArenaPop(Arena* arena, U64 amt)
{
    U64 pos_old = ArenaPos(arena);
    U64 pos_new = pos_old;
    if (amt < pos_old)
    {
        pos_new = pos_old - amt;
    }
    ArenaPopTo(arena, pos_new);
}

ArenaTemp TempBegin(Arena* arena)
{
    U64 pos = ArenaPos(arena);
    ArenaTemp temp = {arena, pos};
    return temp;
}

void TempEnd(ArenaTemp temp)
{
    ArenaPopTo(temp.arena, temp.pos);
}
