#include <core_string.h>
#include <arena.h>

#define STB_SPRINTF_IMPLEMENTATION
#include <stb/stb_sprintf.h>


////////////////////////////////////////////////////////////////
//~ Sebas: String Functions 


//- Sebas: Allocation

String8 PushStr8Copy(Arena* arena, String8 string)
{
    String8 result = {};
    result.size = string.size;
    result.str = PushArray(arena, U8, string.size + 1);
    M_CopyMemory(result.str, string.str, string.size);
    result.str[string.size] = 0;
    return result;
}

String8 PushStr8FV(Arena* arena,char* fmt, va_list args)
{
    String8 result = {};
    va_list args2;
    va_copy(args2, args);
    U32 needed_bytes = ts_stbsp_vsnprintf(0, 0, fmt, args) + 1;
    result.str = PushArrayNoZero(arena, U8, needed_bytes);
    result.size = needed_bytes - 1;
    ts_stbsp_vsnprintf(result.cstr, needed_bytes, fmt, args2);
    return result;
}

String8 PushStr8F(Arena* arena, char* fmt, ...)
{
    String8 result = {};
    va_list args;
    va_start(args, fmt);
    result = PushStr8FV(arena, fmt, args);
    va_end(args);
    return result;
}

String8 PushStr8FillByte(Arena* arena, U64 size, U8 byte)
{
    String8 result = {};
    result.str = PushArrayNoZero(arena, U8, size);
    M_MemorySet(result.str, byte, size);
    result.size = size;
    return result;
}
