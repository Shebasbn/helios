#pragma once

////////////////////////////////////////////
// NOTE(sebas): String Types

struct String8
{
    U8* str;
    U64 size;
};

struct String16
{
    U16* str;
    U64 size;
};

struct String32
{
    U32* str;
    U64 size;
};

typedef U32 StringMatchFlags;
enum
{
    StringMatchFlag_NoCase = 1 << 0,
};

////////////////////////////////////////////
// NOTE(Sebas): String Linked Lists
struct String8Node
{
    String8Node* next;
    String8 string;
};

struct String16Node
{
    String16Node* next;
    String16 string;
};


struct String8List
{
    String8Node* first;
    String8Node* last;
    U64 node_count;
    U64 total_size;
};

struct String16List
{
    String16Node* first;
    String16Node* last;
    U64 node_count;
    U64 total_size;
};

struct String8Join
{
    String8 pre;
    String8 mid;
    String8 post;
};

struct String16Join
{
    String16 pre;
    String16 mid;
    String16 post;
};

////////////////////////////////////////////
// NOTE(sebas): Unicode Helper Types

struct StringDecode
{
    U32 codepoint;
    U32 size;
};

/////////////////////////////////////////////////
// NOTE(sebas): String Contructor Functions

function String8        str8(U8* str, U64 size);
function String8        str8_range(U8* first, U8* opl);
function String8        str8_from_cstr(U8* cstr);
function String8        str8_push_copy(M_Arena* arena, String8 string);

function String16       str16(U16* str, U64 size);
function String16       str16_range(U16* first, U16* opl);
function String16       str16_from_cstr(U16* cstr);
function String16       str16_push_copy(M_Arena* arena, String16 string);

function String32       str32(U32* str, U64 size);
function String32       str32_range(U32* first, U32* opl);
function String32       str32_from_cstr(U32* cstr);

///////////////////////////////////////////////////////
// NOTE(Sebas): String Macros
#define str8_lit(s)     str8((U8*)(s), sizeof(s) - 1)
// TODO(Sebas): Not Sure str_lit macros will work for 16 and 32 wide str literals
#define str16_lit(s)    str16_from_cstr((U16*)s)
#define str32_lit(s)    str32((U32*)(s), sizeof(s) - 1)

#define str_expand(s) (S32)((s).size), ((s).str)
#define str16_expand(s) (S32)((s).size), ((wchar_t*)(s).str)

/////////////////////////////////////////////////
// NOTE(sebas): String Functions

function String8        str8_prefix(String8 str, U64 size);
function String8        str8_postfix(String8 str, U64 size);
function String8        str8_chop(String8 str, U64 amount);
function String8        str8_chop_after_last_slash(String8 str);
function String8        str8_skip(String8 str, U64 amount);
function String8        str8_substr(String8 str, U64 first, U64 opl);

function String16       str16_prefix(String16 str, U64 size);
function String16       str16_postfix(String16 str, U64 size);
function String16       str16_chop(String16 str, U64 amount);
function String16       str16_chop_after_last_slash(String16 str);
function String16       str16_skip(String16 str, U64 amount);
function String16       str16_substr(String16 str, U64 first, U64 opl);

function void           str8_list_push_explicit(String8List* list, String8 string, String8Node* node_memory);
function void           str8_list_push(M_Arena* arena, String8List* list, String8 string);
function String8        str8_join(M_Arena* arena, String8List* list, String8Join* optional_join);
function String8List    str8_split(M_Arena* arena, String8 string, U8* split_characters, U32 count);

function void           str16_list_push_explicit(String16List* list, String16 string, String16Node* node_memory);
function void           str16_list_push(M_Arena* arena, String16List* list, String16 string);
function String16       str16_join(M_Arena* arena, String16List* list, String16Join* optional_join);
function String16List   str16_split(M_Arena* arena, String16 string, U16* split_characters, U32 count);

function String8        str8_pushfv(M_Arena* arena, char* fmt, va_list args);
function String8        str8_pushf(M_Arena* arena, char* fmt, ...);
function void           str8_list_pushf(M_Arena* arena, String8List* list, char* fmt, ...);

////////////////////////////////////////////
// NOTE(sebas): String Comparison Functions

function B32 str8_match(String8 a, String8 b, StringMatchFlags flags);

////////////////////////////////////////////
// NOTE(sebas): Unicode Functions

function StringDecode   str_decode_utf8(U8* str, U32 cap);
function U32            str_encode_utf8(U8* dst, U32 codepoint);
function StringDecode   str_decode_utf16(U16* str, U32 cap);
function U32            str_encode_utf16(U16* dst, U32 codepoint);

function String32 str32_from_str8(M_Arena* arena, String8 string);
function String8 str8_from_str32(M_Arena* arena, String32 string);
function String16 str16_from_str8(M_Arena* arena, String8 string);
function String8 str8_from_str16(M_Arena* arena, String16 string);



////////////////////////////////////////////
// NOTE(sebas): Character Functions

function U8 str8_char_uppercase(U8 c);
function U8 str8_char_lowercase(U8 c);

