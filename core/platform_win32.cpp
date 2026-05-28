#include <platform.h>

#if PLATFORM_WINDOWS

# define WIN32_MEAN_AND_LEAN
# include <windows.h>


struct TimeStamp
{
    LARGE_INTEGER counters;
};

struct PlatformState
{
    HWND window;
    HINSTANCE instance;
    TimeStamp start_up;
};

global PlatformState g_win32_state = {};


B32 PF_StartUp(PlatformHandle* handle, 
               String8 app_name, 
               S32 x, S32 y, 
               S32 width, S32 height)
{
    B32 result = 0;
    
    return result;
}

B32 PF_Shutdown();

U64 PF_PageSize(void)
{
    return 0;
}

void* PF_MemoryReserve(U64 size)
{
    return 0;
}

void PF_MemoryCommit(void* ptr, U64 size);
void PF_MemoryDecommit(void* ptr, U64 size);
void PF_MemoryRelease(void* ptr);

#endif