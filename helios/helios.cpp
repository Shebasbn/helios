#include <common.h>
#include <platform.h>
#include <core_string.h>
#include <arena.h>

#include "helios.h"

#include <stdio.h>

struct Foo
{
    String8 name;
    F32 r, g, b, a;
};

int main()
{
    printf("Test\n");
    PlatformHandle handle = {};
    String8 helios_str = Str8Lit("Helios!");
    String8 arena_name =  Str8Lit("Platform Arena");
    Arena* arena = ArenaAlloc(arena_name);
    B32 is_running = PF_StartUp(&handle, helios_str, 100, 100, 1280, 720);
    
    while(is_running)
    {
        
    }
    
    return 0;
}