/* date = May 27th 2026 9:33 pm */

#ifndef CORE_STRING_H
#define CORE_STRING_H

#include <common.h>

struct String8
{
    union
    {
        U8* str;
        char* cstr;
    };
    U64 size;
};

struct String16
{
    union
    {
        U16* str;
        char16_t* cstr;
    };
    U64 size;
};

struct String32
{
    union
    {
        U32* str;
        char32_t* cstr;
    };
    U64 size;
};

StaticAssert(sizeof(U8) == sizeof(char), "8-bit char size mismatch!");
StaticAssert(sizeof(U16) == sizeof(char16_t), "16-bit char size mismatch!");
StaticAssert(sizeof(U32) == sizeof(char32_t), "32-bit char size mismatch!");

#endif //CORE_STRING_H
