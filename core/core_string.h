/* date = May 27th 2026 9:33 pm */

#ifndef CORE_STRING_H
#define CORE_STRING_H

#include <common.h>

#define STB_SPRINTF_DECORATE(name) ts_stbsp_##name 
#include <stb/stb_sprintf.h>


////////////////////////////////////////////////////////////////
//~ Sebas: String Functions 

//- Sebas: Helpers

force_inline U64 CalcCStrLength(char* cstr) {U64 len = 0; for(;cstr[len]; len+=1); return len;};

//- Sebas: Constructors

force_inline String8 Str8(U8* str, U64 size) { String8 string = { str, size }; return string; }
force_inline String16 Str16(U16* str, U64 size) { String16 string = { str, size }; return string; }
force_inline String32 Str32(U32* str, U64 size) { String32 string = { str, size }; return string; }

#define Str8Zero() (Str8(0, 0))
#define Str8C(cstr) (Str8((U8*)(cstr), CalcCStrLength(cstr)))
#define Str8Lit(str) (Str8((U8*)(str), sizeof(str) - 1))
#define Str8LitComp(str) {(U8*)(str), sizeof(str)-1}

//- Sebas: Substrings

//- Sebas: Allocation

String8 PushStr8Copy(Arena* arena, String8 string);
String8 PushStr8FV(Arena* arena,char* fmt, va_list args);
String8 PushStr8F(Arena* arena, char* fmt, ...);
String8 PushStr8FillByte(Arena* arena, U64 size, U8 byte);

#define Str8VArg(s) (int)(s).size, (s).str

#endif //CORE_STRING_H
