function PixelSize bitmap_get_pixel_size(PixelLayout pixel_layout)
{
    PixelSize result = PixelSize::PixelSize_Unknown;
    switch(pixel_layout)
    {
        case PixelLayout_Packed_XRGB:
        case PixelLayout_Packed_ARGB:
        case PixelLayout_Packed_RGBA:
        case PixelLayout_Byte_BGRA:
        case PixelLayout_Byte_RGBA:
    	{
            result = PixelSize_32Bit;
    	} break;
        case PixelLayout_Byte_R16G16B16A16:
        case PixelLayout_Byte_B16G16R16A16:
        case PixelLayout_Float_RGBA16:
        case PixelLayout_Float_BGRA16:
    	{
            result = PixelSize_64Bit;
    	} break;
        case PixelLayout_Byte_RGB:
        case PixelLayout_Byte_BGR:
    	{
            result = PixelSize_24Bit;
    	} break;
        case PixelLayout_Packed_R5G6B5:
        case PixelLayout_Packed_A1R5G5B5:
    	{
            result = PixelSize_16Bit;
    	} break;
        case PixelLayout_Unknown:
    	default:
    	{
            result = PixelSize_Unknown;
    	} break;
    }
    return result;
}