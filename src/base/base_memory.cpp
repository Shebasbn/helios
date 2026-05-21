//////////////////////////////////////////////////
// NOTE(sebas): Arena Functions
function M_Arena
m_arena_init(void* memory, U64 size)
{
    M_Arena result = {};
    if (memory != NULL && size > 0)
    {
        result.base = (U8*)memory;
        result.pos = 0;
        result.cap = size;
    }
    return result;
}

function void* 
m_arena_push(M_Arena* arena, U64 size, U64 align)
{
    LightAssert(Bool(arena));
    //U64 al_pos = (arena->pos) ? AlignUpPow2(arena->pos, align) : arena->pos;
    U64 al_pos = arena->pos;
    U64 final_pos = al_pos + size;
    void* result = NULL;
    if (final_pos <= arena->cap)
    {
        result = (void*)(arena->base + al_pos);
#if ZERO_INIT
        MemoryZero(result, size);
#endif
        arena->pos = final_pos;
    }
    return result;
}

function void 
m_arena_pop_to(M_Arena* arena, U64 pos)
{
    if (pos < arena->pos)
    {
        arena->pos = pos;
    }
}

function void
m_arena_pop_amount(M_Arena* arena, U64 amount)
{
    if (amount > 0 && arena->pos >= amount)
    {
        m_arena_pop_amount(arena, arena->pos - amount);
    }
}

function void
m_arena_reset(M_Arena* arena)
{
    arena->pos = 0;
}

function M_ArenaTemp
m_begin_temp(M_Arena* arena)
{
    M_ArenaTemp result;
    result.arena = arena;
    result.pos = arena->pos;
    return result;
}

function void
m_end_temp(M_ArenaTemp temp)
{
    m_arena_pop_to(temp.arena, temp.pos);
}

//////////////////////////////////////////////////
// NOTE(sebas): Scratch

threadvar M_Arena* m__scratch_pool[M_SCRATCH_POOL_COUNT] = {};

function void
m_init_scratch(M_Memory* memory)
{
    if (memory->ptr != 0 &&
        memory->size > (M_SCRATCH_POOL_COUNT * sizeof(M_Arena)) &&
        m__scratch_pool[0] == 0)
    {
        M_Arena** scratch_slot = m__scratch_pool;
        U64 size = memory->size / M_SCRATCH_POOL_COUNT;
        U8* raw_base = (U8*)memory->ptr;
        for(U64 i = 0;
            i < M_SCRATCH_POOL_COUNT;
            ++i, ++scratch_slot)
        {
            U8* block_ptr = raw_base + (i * size);
            *scratch_slot = (M_Arena*)block_ptr;
            (*scratch_slot)->base = block_ptr + sizeof(M_Arena);
            (*scratch_slot)->pos = 0;
            (*scratch_slot)->cap = size - sizeof(M_Arena);
        }

    }
}

function M_ArenaTemp
m_get_scratch(M_Arena** conflict_array, U32 count)
{
    M_ArenaTemp result = {};

    if (m__scratch_pool[0] != 0)
    {
        for (U32 i = 0; i < M_SCRATCH_POOL_COUNT; ++i)
        {
            B32 is_non_conflict = true;
            for(U32 j = 0; j < count; ++j)
            {
                if (m__scratch_pool[i] == conflict_array[j])
                {
                    is_non_conflict = false;
                    break;
                }
            }

            if (is_non_conflict)
            {
                result = m_begin_temp(m__scratch_pool[i]);
                break;
            }
        }
    }
    else
    {
        LightAssert(!"Failed To Init Scratch Pool!");
    }
    return result;
}

