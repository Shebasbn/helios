#include <common.h>
#include <platform.h>
#include <core_string.h>
#include <arena.h>

#include "helios.h"

#include <stdio.h>

struct Foo
{
    U64 idx;
    String8 name;
    F32 r, g, b, a;
};

struct FooNode
{
    FooNode* prev;
    FooNode* next;
    Foo v;
    U32 count;
};

int main()
{
    
    printf("Test\n");
    PlatformHandle handle = {};
    String8 helios_str = Str8Lit("Helios!");
    String8 arena_name =  Str8Lit("Platform Arena");
    String8 farena_name =  Str8Lit("Frame Arena");
    Arena* pf_arena = ArenaAlloc(arena_name);
    Arena* farena = ArenaAlloc(farena_name);
    ArenaTemp frame_arena = TempBegin(farena);
    PF_EventList list = {};
    
    B32 is_running = PF_StartUp(pf_arena, &handle, helios_str, 100, 100, 1280, 720);
    while(is_running)
    {
        TempEnd(frame_arena);
        is_running = PF_ProcessPFEvents(frame_arena.arena, handle, &list);
        
    }
    
    PF_Shutdown(pf_arena, handle);
    
    return 0;
}