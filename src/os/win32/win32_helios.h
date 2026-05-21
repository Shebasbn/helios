#pragma once

//////////////////////////////////////
// NOTE(sebas): OS Includes

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


struct W32Dim
{
    S32 width;
    S32 height;
};

struct W32BackBuffer
{
    union
    {
        struct
        {
            BITMAP_FIELDS
        };
        Bitmap bitmap;
    };
    HBITMAP handle;
    BITMAPINFO info;
};

typedef enum 
{
    W32_WINDOW_BORDERLESS = WS_POPUP | WS_VISIBLE,
    W32_WINDOW_FIXED      = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
    W32_WINDOW_RESIZABLE  = WS_OVERLAPPEDWINDOW | WS_VISIBLE
}W32WindowStyle;

typedef enum 
{
    W32_EX_WINDOW_DEFAULT    = WS_EX_APPWINDOW,
    W32_EX_WINDOW_FULLSCREEN = WS_EX_APPWINDOW | WS_EX_TOPMOST
}W32ExWindowStyle;


typedef enum 
{
    W32_SystemPath_CurrentDirectory,
    W32_SystemPath_Data,
    W32_SystemPath_Binary,
    W32_SystemPath_Build,
    W32_SystemPath_COUNT,
} W32SystemPath;


struct Win32State
{
    HWND window;
    HDC device_context;
    W32WindowStyle style;
    W32ExWindowStyle ex_style;
    W32BackBuffer back_buffer;
    union
    {
        struct
        {
            S32 width;
            S32 height;
        };
        W32Dim client;
    };
    F32 aspect_ratio;
    B32 is_resizing;
    PlatformPaths paths;
    GameState game_state;
};

global Win32State* g_w32_state = {};
global U64 g_w32_time_frequency;
global SYSTEM_INFO g_w32_sys_info;
//global PlatformActionType key_binds[MAX_KEY_STATES];


function void w32_init(M_Memory* out_memory);
function S32 WINAPI w32_main(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, int cmd_show);


//function B32  w32_register_window_class(HINSTANCE instance, String8 class_name)
//function HWND w32_create_window(Win32State* state)
function void w32_resize_DIB_section(W32BackBuffer* buffer, HDC device_context , S32 width, S32 height);
function String16 w32_file_path(M_Arena* arena, W32SystemPath path);