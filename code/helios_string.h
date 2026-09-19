/* date = September 16th 2026 9:49 am */

#ifndef HELIOS_STRING_H
#define HELIOS_STRING_H

////////////////////////////////////////////////////////////////
//~ Sebas: String Types

struct string8
{
  u8* str;
  u64 size;
};

struct string16
{
  u16* str;
  u64  size;
};

struct string32
{
  u32* str;
  u64  size;
};

////////////////////////////////////////////////////////////////
//~ Sebas: String Helper Functions

////////////////////////////////////////////////////////////////
//~ Sebas: String Macros

#define Str8(str, size) { (u8*)(str), (u64)(size) }
#define Str8Lit(str) Str8((str), (u64)strlen(str))


#endif //HELIOS_STRING_H
