////////////////////////////////
// NOTE(sebas): Symbolic Constant Functions

function OperatingSystem 
operating_system_from_context(void)
{
    OperatingSystem result = OperatingSystem_NULL;
#if OS_WINDOWS
    result = OperatingSystem_Windows;
#elif OS_LINUX
    result = OperatingSystem_Linux;
#elif OS_MAC
    result = OperatingSystem_Mac;
#endif
    return result;
}

function Architecture 
architecture_from_context(void)
{
    Architecture result = Architecture_NULL;
#if ARCH_X64
    result = Architecture_X64;
#elif ARCH_X86
    result = Architecture_X86;
#elif ARCH_ARM
    result = Architecture_ARM;
#elif ARCH_ARM64
    result = Architecture_ARM64;
#endif
    return result;
}

function char* 
string_from_operating_system(OperatingSystem os)
{
    char* result = "(null)";
    switch (os)
    {
        case OperatingSystem_Windows:
        {
            result = "windows";
        } break;
        case OperatingSystem_Linux:
        {
            result = "linux";
        } break;
        case OperatingSystem_Mac:
        {
            result = "mac";
        } break;
        case OperatingSystem_NULL:
        case OperatingSystem_COUNT:
        {
            LightAssert(!"Invalid OS!");
        } break;
    }
    return result;
}

function char* 
string_from_architecture(Architecture arch)
{
    char* result = "(null)";
    switch (arch)
    {
        case Architecture_X64:
        {
            result = "x64";
        } break;
        case Architecture_X86:
        {
            result = "x86";
        } break;
        case Architecture_ARM:
        {
            result = "arm";
        } break;
        case Architecture_ARM64:
        {
            result = "arm64";
        } break;
        case Architecture_NULL:
        case Architecture_COUNT:
        {
            LightAssert(!"Invalid architecture!");
        } break;
    }
    return result;
}

function char* 
string_from_month(Month month)
{
    char* result = "(null)";
    switch (month)
    {
        case Month_Jan:
        {
            result = "jan";
        } break;
        case Month_Feb:
        {
            result = "feb";
        } break;
        case Month_Mar:
        {
            result = "mar";
        } break;
        case Month_Apr:
        {
            result = "apr";
        } break;
        case Month_May:
        {
            result = "may";
        } break;
        case Month_Jun:
        {
            result = "jun";
        } break;
        case Month_Jul:
        {
            result = "jul";
        } break;
        case Month_Aug:
        {
            result = "aug";
        } break;
        case Month_Sep:
        {
            result = "sep";
        } break;
        case Month_Oct:
        {
            result = "oct";
        } break;
        case Month_Nov:
        {
            result = "nov";
        } break;
        case Month_Dec:
        {
            result = "dec";
        } break;
    }
    return result;
}

function char* 
string_from_day_of_week(DayOfWeek day_of_week)
{
    char* result = "(null)";
    switch (day_of_week)
    {
        case Day_Sun:
        {
            result = "sunday";
        } break;
        case Day_Mon:
        {
            result = "monday";
        } break;
        case Day_Tue:
        {
            result = "tuesday";
        } break;
        case Day_Wed:
        {
            result = "wednesday";
        } break;
        case Day_Thu:
        {
            result = "thursday";
        } break;
        case Day_Fri:
        {
            result = "friday";
        } break;
        case Day_Sat:
        {
            result = "saturday";
        } break;
        default: break;
    }
    return result;
}

////////////////////////////////
// NOTE(sebas): Float Constant Functions

function F32 
inf_F32(void)
{
    union { F32 f; U32 u; } r;
    r.u = 0x7f800000;
    return r.f;
}

function F32 
neg_inf_F32(void)
{
    union { F32 f; U32 u; } r;
    r.u = 0xff800000;
    return r.f;
}

function F64 
inf_F64(void)
{
    union { F64 f; U64 u; } r;
    r.u = 0x7ff0000000000000;
    return r.f;
}

function F64 
neg_inf_F64(void)
{
    union { F64 f; U64 u; } r;
    r.u = 0xfff0000000000000;
    return r.f;
}

////////////////////////////////
// NOTE(sebas): Math Functions

function F32 
abs_F32(F32 x)
{
    union { F32 f; U32 u; } r;
    r.f = x;
    r.u &= 0x7fffffff;
    return r.f;
}

function F64 
abs_F64(F64 x)
{
    union { F64 f; U64 u; } r;
    r.f = x;
    r.u &= 0x7fffffffffffffff;
    return r.f;
}

#include <math.h>

function F32 
sqrt_F32(F32 x)
{
    return sqrtf(x);
}

function F32 
sin_F32(F32 x)
{
    return sinf(x);
}

function F32 
cos_F32(F32 x)
{
    return cosf(x);
}

function F32 
tan_F32(F32 x)
{
    return tanf(x);
}

function F32 
ln_F32(F32 x)
{
    return logf(x);
}

function F64 
sqrt_F64(F64 x)
{
    return sqrt(x);
}

function F64 
sin_F64(F64 x)
{
    return sin(x);
}

function F64 
cos_F64(F64 x)
{
    return cos(x);
}

function F64 
tan_F64(F64 x)
{
    return tan(x);
}

function F64
ln_F64(F64 x)
{
    return log(x);
}

function F32 
lerp(F32 a, F32 t, F32 b)
{
    F32 x = a + (b - a) * t;
    return x;
}

function F32 
unlerp(F32 a, F32 x, F32 b)
{
    F32 t = 0.0f;

    if (a != b)
    {
        t = (x - a) / (b - a);
    }
    return t;
}

