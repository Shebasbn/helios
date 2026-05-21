////////////////////////////////////////////
// NOTE(sebas): String Contructor Functions

function String8 
str8(U8* str, U64 size)
{
    String8 result = { str, size };
    return result;
}

function String8 
str8_range(U8* first, U8* opl)
{
    String8 result = { first, (U64)(opl - first) };
    return result;
}

function String8
str8_from_cstr(U8* cstr)
{
    U8* ptr = cstr;
    for(;*ptr != 0;++ptr);
    String8 result = {cstr, U64(ptr - cstr)};
    return result;
}

function String8
str8_push_copy(M_Arena* arena, String8 string)
{
    String8 result = {};
    result.str = PushArray(arena, U8, string.size + 1);
    if (Bool(MemoryCopy(result.str, string.str, string.size * sizeof(U8))))
    {
        result.size = string.size;
        result.str[string.size] = 0;
        return result;
    }
}

function String16 
str16(U16* str, U64 size)
{
    String16 result = { str, size };
    return result;
}

function String16 
str16_range(U16* first, U16* opl)
{
    String16 result = { first, (U64)(opl - first) };
    return result;
}

function String16
str16_from_cstr(U16* cstr)
{
    U16* ptr = cstr;
    for(;*ptr != 0;++ptr);
    String16 result = {cstr, U64(ptr - cstr)};
    return result;
}

function String16
str16_push_copy(M_Arena* arena, String16 string)
{
    String16 result = {};
    M_ArenaTemp restore_point = m_begin_temp(arena);
    result.str = PushArray(arena, U16, string.size + 1);
    if (Bool(MemoryCopy(result.str, string.str, string.size * sizeof(U16))))
    {
        result.size = string.size;
        result.str[string.size] = 0;
    }
    else
    {
        m_end_temp(restore_point);
    }
    return result;
}

function String32 
str32(U32* str, U64 size)
{
    String32 result = { str, size };
    return result;
}

function String32 
str32_range(U32* first, U32* opl)
{
    String32 result = { first, (U64)(opl - first) };
    return result;
}

function String32
str32_from_cstr(U32* cstr)
{
    U32* ptr = cstr;
    for(;*ptr != 0;++ptr);
    String32 result = {cstr, U64(ptr - cstr)};
    return result;
}

function String8 
str8_prefix(String8 str, U64 size)
{
    U64 size_clamped = ClampTop(size, str.size);
    String8 result = { str.str, size_clamped };
    return result;
}

function String8 
str8_postfix(String8 str, U64 size)
{
    U64 size_clamped = ClampTop(size, str.size);
    U64 skip_to = str.size - size_clamped;
    String8 result = { str.str + skip_to, size_clamped };
    return result;
}

function String8 
str8_chop(String8 str, U64 amount)
{
    U64 amount_clamped = ClampTop(amount, str.size);
    U64 remaining_size = str.size - amount_clamped;
    String8 result = { str.str, remaining_size };
    return result;
}

function String8 
str8_skip(String8 str, U64 amount)
{
    U64 amount_clamped = ClampTop(amount, str.size);
    U64 remaining_size = str.size - amount_clamped;
    String8 result = { str.str + amount_clamped, remaining_size };
    return result;
}

function String8
str8_substr(String8 str, U64 first, U64 opl)
{
    U64 actual_first = ClampTop(first, str.size);
    U64 actual_opl = Clamp(actual_first, opl, str.size);
    String8 result = {};
    result.str = str.str + actual_first;
    result.size = actual_opl - actual_first;
    return result;
}

function String16 
str16_prefix(String16 str, U64 size)
{
    U64 size_clamped = ClampTop(size, str.size);
    String16 result = { str.str, size_clamped };
    return result;
}

function String16 
str16_postfix(String16 str, U64 size)
{
    U64 size_clamped = ClampTop(size, str.size);
    U64 skip_to = str.size - size_clamped;
    String16 result = { str.str + skip_to, size_clamped };
    return result;
}

function String16 
str16_chop(String16 str, U64 amount)
{
    U64 amount_clamped = ClampTop(amount, str.size);
    U64 remaining_size = str.size - amount_clamped;
    String16 result = { str.str, remaining_size };
    return result;
}

function String16
str16_chop_after_last_slash(String16 str)
{
    String16 result = {};
    if (str.size > 0)
    {
        U64 pos = str.size - 1;
        if (str.str[pos] == '\\' || str.str[pos] == '/')
        {
            --pos;
        }
        for (;pos > 0;--pos)
        {
            if (str.str[pos] == '\\' || str.str[pos] == '/')
            {
                pos++;
                result.str = str.str;
                result.size = pos;
                break;
            }
        }
    }
    return result;
}

function String16 
str16_skip(String16 str, U64 amount)
{
    U64 amount_clamped = ClampTop(amount, str.size);
    U64 remaining_size = str.size - amount_clamped;
    String16 result = { str.str + amount_clamped, remaining_size };
    return result;
}

function String16
str16_substr(String16 str, U64 first, U64 opl)
{
    U64 actual_first = ClampTop(first, str.size);
    U64 actual_opl = Clamp(actual_first, opl, str.size);
    String16 result = {};
    result.str = str.str + actual_first;
    result.size = actual_opl - actual_first;
    return result;
}

function void 
str8_list_push_explicit(String8List* list, 
                        String8 string,
                        String8Node* node_memory)
{
    node_memory->string = string;
    SLLQueuePush(list->first, list->last, node_memory);
    list->node_count++;
    list->total_size += string.size;
}

function void 
str8_list_push(M_Arena* arena, 
               String8List* list, 
               String8 string)
{
    String8Node* node = PushStruct(arena, String8Node);
    str8_list_push_explicit(list, string, node);
}

function String8
str8_join(M_Arena* arena, 
          String8List* list,
          String8Join* optional_join)
{
    local String8Join dummy_join = {};
    String8Join* join = optional_join;
    if (join == 0)
    {
        join = &dummy_join;
    }

    U64 mid_count = (list->node_count > 1) ? (list->node_count - 1) : 0;
    U64 size = (join->pre.size + 
               join->post.size +
               join->mid.size * mid_count + 
               list->total_size);

    U8* str = PushArray(arena, U8, size + 1);
    U8* ptr = str;

    MemoryCopy(ptr, join->pre.str, join->pre.size);
    ptr += join->pre.size;

    B32 is_mid = false;
    for (String8Node* node = list->first;
         node != 0;
         node = node->next)
    {
        if (Bool(is_mid))
        {
            MemoryCopy(ptr, join->mid.str, join->mid.size);
            ptr += join->mid.size;
        }

        MemoryCopy(ptr, node->string.str, node->string.size);
        ptr += node->string.size;
        is_mid = true;  
    }

    MemoryCopy(ptr, join->post.str, join->post.size);
    ptr += join->post.size;

    *ptr = 0;
    String8 result = { str, size };
    return result;
}

function String8List
str8_split(M_Arena* arena,
           String8 string,
           U8* splits,
           U32 count)
{
    String8List result = {};

    U8* ptr = string.str;
    U8* word_first = ptr;
    U8* opl = string.str + string.size;
    for (; ptr < opl; ptr++)
    {
        U8 byte = *ptr;
        B32 is_split_byte = false;
        for (U32 i = 0; i < count; ++i)
        {
            if (byte == splits[i])
            {
                is_split_byte = true;
                break;
            }
        }

        if (Bool(is_split_byte))
        {
            if (word_first < ptr)
            {
                str8_list_push(arena, &result, str8_range(word_first, ptr));
            }
            word_first = ptr + 1;
        }
    }

    if (word_first < ptr)
    {
        str8_list_push(arena, &result, str8_range(word_first, ptr));
    }

    return result;
}

function void 
    str16_list_push_explicit(String16List* list, 
                             String16 string,
                             String16Node* node_memory)
{
    node_memory->string = string;
    SLLQueuePush(list->first, list->last, node_memory);
    list->node_count++;
    list->total_size += string.size;
}

function void 
    str16_list_push(M_Arena* arena, 
                    String16List* list, 
                    String16 string)
{
    String16Node* node = PushStruct(arena, String16Node);
    str16_list_push_explicit(list, string, node);
}

function String16
    str16_join(M_Arena* arena, 
               String16List* list,
               String16Join* optional_join)
{
    local String16Join dummy_join = {};
    String16Join* join = optional_join;
    if (join == 0)
    {
        join = &dummy_join;
    }

    U64 mid_count = (list->node_count > 1) ? (list->node_count - 1) : 0;
    U64 size = (join->pre.size + 
               join->post.size +
               join->mid.size * mid_count + 
               list->total_size);

    U16* str = PushArray(arena, U16, size + 1);
    U16* ptr = str;

    MemoryCopyTyped(ptr, join->pre.str, join->pre.size);
    ptr += join->pre.size;

    B32 is_mid = false;
    for (String16Node* node = list->first;
        node != 0;
        node = node->next)
    {
        if (Bool(is_mid))
        {
            MemoryCopyTyped(ptr, join->mid.str, join->mid.size);
            ptr += join->mid.size;
        }

        MemoryCopyTyped(ptr, node->string.str, node->string.size);
        ptr += node->string.size;
        is_mid = true;  
    }

    MemoryCopyTyped(ptr, join->post.str, join->post.size);
    ptr += join->post.size;

    *ptr = 0;
    String16 result = { str, size };
    return result;
}

function String16List
str16_split(M_Arena* arena,
                String16 string,
                U16* splits,
                U32 count)
{
    String16List result = {};

    U16* ptr = string.str;
    U16* word_first = ptr;
    U16* opl = string.str + string.size;
    for (; ptr < opl; ptr++)
    {
        U16 byte = *ptr;
        B32 is_split_byte = false;
        for (U32 i = 0; i < count; ++i)
        {
            if (byte == splits[i])
            {
                is_split_byte = true;
                break;
            }
        }

        if (Bool(is_split_byte))
        {
            if (word_first < ptr)
            {
                str16_list_push(arena, &result, str16_range(word_first, ptr));
            }
            word_first = ptr + 1;
        }
    }

    if (word_first < ptr)
    {
        str16_list_push(arena, &result, str16_range(word_first, ptr));
    }

    return result;
}

function String8
str8_pushfv(M_Arena* arena, char* fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);

    U64 buffer_size = 1024;
    U8* buffer = (U8*)PushSize(arena, buffer_size);
    U64 actual_size = (U64)vsnprintf((char*)buffer, buffer_size, fmt, args);

    String8 result = {};
    if (actual_size < buffer_size)
    {
        m_arena_pop_amount(arena, buffer_size - actual_size - 1);
        result = str8(buffer, actual_size);
    }
    else
    {
        m_arena_pop_amount(arena, buffer_size);
        U8* fixed_buffer = (U8*)PushSize(arena, actual_size + 1);
        U64 final_size = (U64)vsnprintf((char*)fixed_buffer, actual_size, fmt, args2);
        result = str8(fixed_buffer, final_size);
    }

    va_end(args2);
    return result;
}

function String8
str8_pushf(M_Arena* arena, char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    String8 result = str8_pushfv(arena, fmt, args);
    va_end(args);
    return result;
}

function void
str8_list_pushf(M_Arena* arena, String8List* list, char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    String8 string = str8_pushfv(arena, fmt, args);
    va_end(args);
    str8_list_push(arena, list, string);
}

////////////////////////////////////////////
// NOTE(sebas): String Comparison Functions

function B32 
str8_match(String8 a, String8 b, StringMatchFlags flags=0)
{
    B32 result = false;
    if (a.size == b.size)
    {
        result = true;
        B32 no_case = ((flags & StringMatchFlag_NoCase) != 0);
        for (U64 i = 0; i < a.size; ++i)
        {
            U8 ac = a.str[i];
            U8 bc = b.str[i];
            if (no_case)
            {
                ac = str8_char_uppercase(ac);
                bc = str8_char_uppercase(bc);
            }
            if (ac != bc)
            {
                result = false;
                break;
            }
        }
    }
    return result;
}

////////////////////////////////////////////
// NOTE(sebas): Unicode Functions

function StringDecode 
str_decode_utf8(U8* str, U32 cap)
{
    local U8 utf8_length[] ={
    // 00 01 10 11
        1, 1, 1, 1, // 000xx
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        0, 0, 0, 0, // 100xx
        0, 0, 0, 0, 
        2, 2, 2, 2, // 110xx
        3, 3,       // 1110x
        4,          // 11110
        0           // 11111
    };

    local U8 first_byte_mask[] { 0, 0x7F, 0x1F, 0x0F, 0x07 };

    StringDecode result = {};
    if (cap > 0)
    {
        U8 byte = str[0];
        U8 l = utf8_length[byte >> 3];

        result.codepoint = '#';
        result.size = 1;
        if (l > 0 && l <= cap)
        {
            U32 cp = (U32)(byte & first_byte_mask[l]);
            for (U32 i = 1; i < l; ++i)
            {
                cp <<= 6;
                cp |= str[i] & 0x3F;
            }

            result.codepoint = cp;
            result.size = l;

        }
    } 
    return result;
}

function U32
str_encode_utf8(U8* dst, U32 codepoint)
{
    local U8 first_byte_mask[] { 0, 0x7F, 0x1F, 0x0F, 0x07 };

    U32 size = 0;
    if (codepoint < (1 << 7))
    {
        size = 1;
        dst[0] = (U8)codepoint;
    }
    else if (codepoint < (1 << 11))
    {
        size = 2;
        dst[0] = (U8)(0xC0 | (codepoint >> 6));
        dst[1] = (U8)(0x80 | (codepoint & 0x3f));
    }
    else if (codepoint < (1 << 16))
    {
        size = 3;
        dst[0] = (U8)(0xE0 | (codepoint >> 12));
        dst[1] = (U8)(0x80 | ((codepoint >> 6) & 0x3f));
        dst[2] = (U8)(0x80 | (codepoint & 0x3f));
    }
    else if (codepoint < (1 << 21))
    {
        size = 4;
        dst[0] = (U8)(0xF0 | (codepoint >> 18));
        dst[1] = (U8)(0x80 | ((codepoint >> 12) & 0x3f));
        dst[2] = (U8)(0x80 | ((codepoint >> 6) & 0x3f));
        dst[3] = (U8)(0x80 | (codepoint & 0x3f));
    }
    else
    {
        dst[0] = '#';
        size = 1;
    }

    return size;
}


function StringDecode
str_decode_utf16(U16* str, U32 cap)
{
    StringDecode result = {'#', 1};
    if (cap > 0)
    {
        U16 x = str[0];
        //U8 mask = byte >> 10;

        if (x < 0xD800 || 0xDFFF < x)
        {
            result.codepoint = (U32)x;
        }
        else if (cap >= 2)
        {
            U16 y = str[1];
            if (0xD800 <= x && x < 0xDC00 &&
                0xDC00 <= y && y < 0xE000)
            {
                U16 xj = (U16)(x - 0xD800);
                U16 yj = (U16)(y - 0xDC00);
                U32 xy = (((U32)(xj) << 10) | (U32)yj);
                result.codepoint = xy + 0x10000;
                result.size = 2;
            }
        }
    }
    return result;
}

function U32
str_encode_utf16(U16* dst, U32 codepoint)
{
    U32 size = 0;
    if (codepoint < 0x10000)
    {
        dst[0] = (U16)codepoint;
        size = 1;
    }
    else
    {
        U32 cp = codepoint - 0x10000;
        dst[0] = (U16)(0xD800 + (cp >> 10));
        dst[1] = (U16)(0xDC00 + (cp & 0x3FF));
        size = 2;
    }
    return size;
}

function String32
str32_from_str8(M_Arena* arena, String8 string)
{
    U64 alloc_count = string.size + 1;
    U32* memory = PushArray(arena, U32, alloc_count);
    U32* dptr = memory;
    U8* ptr = string.str;
    U8* opl = string.str + string.size;
    while (ptr < opl)
    {
        StringDecode decode = str_decode_utf8(ptr, (U32)(opl - ptr));
        *dptr++ = (U32)decode.codepoint;
        ptr += decode.size;
    }
    *dptr = 0;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));

    String32 result = { memory, string_count };
    return result;
}

function String8
str8_from_str32(M_Arena* arena, String32 string)
{
    U64 alloc_count = string.size * 4 + 1;
    U8* memory = PushArray(arena, U8, alloc_count);
    U8* dptr = memory;
    U32* ptr = string.str;
    U32* opl = string.str + string.size;
    while (ptr < opl)
    {
        U32 size = str_encode_utf8(dptr, *ptr++);
        dptr += size;
    }
    *dptr = 0;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));

    String8 result = { memory, string_count };
    return result;
}

function String16
str16_from_str8(M_Arena* arena, String8 string)
{
    U64 alloc_count = string.size + 1;
    U16* memory = PushArray(arena, U16, alloc_count);
    U16* dptr = memory;
    U8* ptr = string.str;
    U8* opl = string.str + string.size;
    while (ptr < opl)
    {
        StringDecode decode = str_decode_utf8(ptr, (U32)(opl - ptr));
        U32 enc_size = str_encode_utf16(dptr, decode.codepoint);
        dptr += enc_size;
        ptr += decode.size;
    }
    *dptr = 0;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));

    String16 result = { memory, string_count };
    return result;
}

function String8
str8_from_str16(M_Arena* arena, String16 string)
{
    U64 alloc_count = string.size * 2 + 1;
    LightAssert(alloc_count * sizeof(U8) >= string.size * sizeof(*string.str) && 
           "Failed to Allocate enough memory!");
    U8* memory = PushArray(arena, U8, alloc_count);
    U8* dptr = memory;
    U16* ptr = string.str;
    U16* opl = string.str + string.size;
    while (ptr < opl)
    {
        StringDecode decode = str_decode_utf16(ptr, (U32)(opl - ptr));
        U32 enc_size = str_encode_utf8(dptr, decode.codepoint);
        ptr += decode.size;
        dptr += enc_size;
    }
    *dptr = 0;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));

    String8 result = { memory, string_count };
    return result;
}

////////////////////////////////////////////
// NOTE(sebas): Character Functions

function U8 
str8_char_uppercase(U8 c)
{
    if ('a' <= c && c <= 'z')
    {
        c += (U8)('A' - 'a');
    }
    return c;
}

function U8
str8_char_lowercase(U8 c)
{
    if ('A' <= c && c <= 'Z')
    {
        c += (U8)('a' - 'A');
    }
    return c;
}
