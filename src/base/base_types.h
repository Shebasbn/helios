#pragma once

////////////////////////////////
// NOTE(sebas): Basic Constants

global read_only S8  Min_S8  = (S8)0x80u;
global read_only S16 Min_S16 = (S16)0x8000u;
global read_only S32 Min_S32 = (S32)0x80000000u;
global read_only S64 Min_S64 = (S64)0x8000000000000000llu;

global read_only S8  Max_S8  = (S8)0x7fu;
global read_only S16 Max_S16 = (S16)0x7fffu;
global read_only S32 Max_S32 = (S32)0x7fffffffu;
global read_only S64 Max_S64 = (S64)0x7fffffffffffffffllu;

global read_only U8  Max_U8  = 0xff;
global read_only U16 Max_U16 = 0xffff;
global read_only U32 Max_U32 = 0xffffffff;
global read_only U64 Max_U64 = 0xffffffffffffffffllu;

global read_only F32 Machine_Epsilon_F32 = (F32)1.1920929e-7;
global read_only F32 Pi_F32 = 3.14159265359f;
global read_only F32 Tau_F32 = 6.28318530718f;
global read_only F32 E_F32 = 2.71828182846f;
global read_only F32 Gold_Big_F32 = 1.61803398875f;
global read_only F32 Gold_Small_F32 = 0.61803398875f;

global read_only F64 Machine_Epsilon_F64 = 2.220446e-16;
global read_only F64 Pi_F64 = 3.14159265359;
global read_only F64 Tau_F64 = 6.28318530718;
global read_only F64 E_F64 = 2.71828182846;
global read_only F32 Gold_Big_F64 = 1.61803398875f;
global read_only F64 Gold_Small_F64 = 0.61803398875;

////////////////////////////////
// NOTE(sebas): Symbolic Constants

enum Axis
{
    Axis_X,
    Axis_Y,
    Axis_Z,
    Axis_W,
};

enum Side
{
    Side_Min,
    Side_Max,
};

enum OperatingSystem
{
    OperatingSystem_NULL,
    OperatingSystem_Windows,
    OperatingSystem_Linux,
    OperatingSystem_Mac,
    OperatingSystem_COUNT
};

enum Architecture
{
    Architecture_NULL,
    Architecture_X64,
    Architecture_X86,
    Architecture_ARM,
    Architecture_ARM64,
    Architecture_COUNT
};

enum Month
{
    Month_Jan,
    Month_Feb,
    Month_Mar,
    Month_Apr,
    Month_May,
    Month_Jun,
    Month_Jul,
    Month_Aug,
    Month_Sep,
    Month_Oct,
    Month_Nov,
    Month_Dec,
};

enum DayOfWeek
{
    Day_Sun,
    Day_Mon,
    Day_Tue,
    Day_Wed,
    Day_Thu,
    Day_Fri,
    Day_Sat,
};


////////////////////////////////
// NOTE(sebas): Basic Compound Types
union RangeF32
{
    struct
    {
        F32 min;
        F32 max;
    };
    F32 v[2];
};

union RangeU64
{
    struct
    {
        U64 min;
        U64 max;
    };
    struct
    {
        U64 first;
        U64 opl;
    };
    U64 v[2];
};

union RectS32
{
    struct
    {
        V2S32 min;
        V2S32 max;
    };
    struct
    {
        V2S32 p0;
        V2S32 p1;
    };
    struct
    {
        S32 x0;
        S32 y0;
        S32 x1;
        S32 y1;
    };
    V2S32 p[2];
    S32 v[4];
};

union RectF32
{
    struct
    {
        V2F32 min;
        V2F32 max;
    };
    struct
    {
        V2F32 p0;
        V2F32 p1;
    };
    struct
    {
        F32 x0;
        F32 y0;
        F32 x1;
        F32 y1;
    };
    V2F32 p[2];
    F32 v[4];
};

////////////////////////////////
// NOTE(sebas): Symbolic Constant Functions

function OperatingSystem operating_system_from_context(void);
function Architecture architecture_from_context(void);

function char* string_from_operating_system(OperatingSystem os);
function char* string_from_architecture(Architecture arch);
function char* string_from_month(Month month);
function char* string_from_day_of_week(DayOfWeek day_of_week);

/////////////////////////////////////////
// NOTE(sebas): Float Constant Functions

function F32 inf_F32(void);
function F32 neg_inf_F32(void);
function F64 inf_F64(void);
function F64 neg_inf_F64(void);

////////////////////////////////
// NOTE(sebas): Math Functions

function F32 abs_F32(F32 x);
function F64 abs_F64(F64 x);

function F32 sqrt_F32(F32 x);
function F32 sin_F32(F32 x);
function F32 cos_F32(F32 x);
function F32 tan_F32(F32 x);
function F32 ln_F32(F32 x);

function F64 sqrt_F64(F64 x);
function F64 sin_F64(F64 x);
function F64 cos_F64(F64 x);
function F64 tan_F64(F64 x);
function F64 ln_F64(F64 x);

function F32 lerp(F32 a, F32 t, F32 b);
function F32 unlerp(F32 a, F32 x, F32 b);

////////////////////////////////
// NOTE(sebas): Compund Type Functions

