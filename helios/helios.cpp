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
    String8 arena_name =  Str8Lit("Platform Arena");
    Arena* pf_arena = ArenaAlloc(arena_name);
    String8 helios_str = Str8Lit("Helios!");
    PlatformHandle handle;
    B32 is_running = PF_StartUp(pf_arena, &handle, helios_str, 100, 100, 1280, 720);
    
    PF_CreateThread(pf_arena);
    
    printf("Test\n");
    
    String8 farena_name =  Str8Lit("Frame Arena");
    
    Arena* farena = ArenaAlloc(farena_name);
    ArenaTemp frame_arena = TempBegin(farena);
    PF_EventList list = {};
    
    TimeStamp frame_start = {};
    TimeStamp frame_end = {};
    TimeStamp last_frame_start = {};
    F64 target_frame_time = 1.0f / 60.0f; 
    F64 last_processing_time = 0.0; 
    
    while(is_running)
    {
        F64 sleep_time = target_frame_time - last_processing_time;
        if (sleep_time)
        {
            PF_TimeSleep(sleep_time);
        }
        
        
        frame_start = PF_TimeStampCreate();
        F64 delta_time = PF_TimeDeltaInSeconds(last_frame_start, frame_start);
        // TODO(Sebas): Clamp dt if difference form target is less than 0.0001
        last_frame_start = frame_start;
        
        if (list.count != 0)
        {
            TempEnd(frame_arena);
            list.first = 0;
            list.last = 0;
            list.count = 0;
        }
        
        is_running = PF_ProcessPFEvents(frame_arena.arena, handle, &list);
        
        printf("target_frame_time: %f, dt = %f\n", target_frame_time, delta_time);
        
        frame_end = PF_TimeStampCreate();
        last_processing_time = PF_TimeDeltaInSeconds(frame_start, frame_end);
    }
    
    PF_Shutdown(pf_arena, handle);
    
    return 0;
}