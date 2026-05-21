#pragma once

struct PlatformPaths
{
    String16 current_dir;
    String16 root;
    String16 bin;
    String16 data;
};

struct GameMemory
{
    void* perm_storage;
    U64 perm_storage_size;
    B32 is_init;
};

struct GameState
{
    GameMemory memory;
    B32 game_running;
    GI_InputState input_state;
};

enum PixelSize : U8
{
    PixelSize_Unknown = 0,
    PixelSize_16Bit   = 16,  // Retro / Packed
    PixelSize_24Bit   = 24,  // Disk storage / Packed
    PixelSize_32Bit   = 32,  // Modern Standard / Packed
    PixelSize_64Bit   = 64,  // HDR Half-Float
};

enum PixelLayout : U8
{
    PixelLayout_Unknown,
    
    // 32-bit Packed Layouts (Named by integer register order on Little-Endian)
    PixelLayout_Packed_XRGB, // 0xXXRRGGBB (Windows DIB native, ignores alpha)
    PixelLayout_Packed_ARGB, // 0xAARRGGBB (Windows DIB standard with transparency)
    PixelLayout_Packed_RGBA, // 0xRRGGBBAA 
    
    // 32-bit Byte-Stream Layouts (Named by literal byte order in memory)
    PixelLayout_Byte_BGRA,   // Native memory order for Windows DIB 
    PixelLayout_Byte_RGBA,   // Native memory order for OpenGL / PNG
    
    // 24-bit Layouts
    PixelLayout_Byte_RGB,    // Standard on-disk BMP/TGA layout
    PixelLayout_Byte_BGR,
    
    // 16-bit Packed Layouts
    PixelLayout_Packed_R5G6B5,
    PixelLayout_Packed_A1R5G5B5,

    // 64-bit Deep-Color Integer Layouts (16-bits per channel)
    PixelLayout_Byte_R16G16B16A16,        // Standard PNG-16 / TIFF-16 channel order
    PixelLayout_Byte_B16G16R16A16,        // Direct3D/WIC Deep-color equivalent
    
    // 64-bit Floating-Point HDR Layouts (16-bit Half-Float per channel)
    PixelLayout_Float_RGBA16,             // EXR / OpenGLES half-float texture format
    PixelLayout_Float_BGRA16,             // Direct3D (DXGI_FORMAT_R16G16B16A16_FLOAT)
};

#define BITMAP_FIELDS           \
    void* memory;               \
    U64 size;                   \
    S32 width;                  \
    S32 height;                 \
    S32 pitch;                  \
    U32 bytes_per_pixel;        \
    PixelSize pixel_size;       \
    PixelLayout layout;        

struct Bitmap
{
    BITMAP_FIELDS
};

function PixelSize bitmap_get_pixel_size(PixelLayout pixel_layout);

shared_export void game_update_and_render(GameState* state);