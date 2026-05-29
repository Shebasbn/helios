/* date = May 27th 2026 6:20 pm */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <common.h>
//#include <core_string.h>

////////////////////////////////////////////////////////////////
//~ Sebas: Platform System 

typedef struct PlatformState PlatformState;

struct PlatformHandle
{
    PlatformState* state;
};


B32 PF_StartUp(PlatformHandle* handle, String8 app_name, S32 x, S32 y, S32 width, S32 height);
B32 PF_Shutdown();

U64 PF_PageSize(void);
U64 PF_AllocGranularity(void);

void* PF_MemoryReserve(U64 size);
void PF_MemoryCommit(void* ptr, U64 size);
void PF_MemoryDecommit(void* ptr, U64 size);
void PF_MemoryRelease(void* ptr);


////////////////////////////////////////////////////////////////
//~ Sebas: Platform Event System 

struct PlatformEvent
{
    PlatformEvent* next;
    PlatformEvent* prev;
};


void PF_ProcessPFEvents(void);

////////////////////////////////////////////////////////////////
//~ Sebas: Platform Time Types and Functions

typedef struct TimeStamp TimeStamp;

struct PF_DateTime
{
    U16 year;         // e.g., 2026
    U8  month;        // 1 - 12 (January = 1)
    U8  day;          // 1 - 31
    U8  hour;         // 0 - 23
    U8  minute;       // 0 - 59
    U8  second;       // 0 - 59
    U16 millisecond;  // 0 - 999 (Crucial for high-accuracy log files)
};

TimeStamp PF_TimeStampCreate(void);
F64 PF_TimeGetHardwareFrequency(void);
F64 PF_TimeDeltaInSeconds(TimeStamp start, TimeStamp end);
F64 PF_TimeSinceStartUpSec(void);
void PF_TimeGetSystemTime(PF_DateTime* out_time);

void PF_TimeSleep(F64 seconds);


#endif //PLATFORM_H
