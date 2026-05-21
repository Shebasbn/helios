#pragma once

union V2S32
{
    struct
    {
        S32 x;
        S32 y;
    };
    struct
    {
        S32 w;
        S32 h;
    };
    S32 v[2];
};

union V2F32
{
    struct
    {
        F32 x;
        F32 y;
    };
    F32 v[2];
};

union V3F32
{
    struct
    {
        F32 x;
        F32 y;
        F32 z;
    };
    F32 v[3];
};

union V4F32
{
    struct
    {
        F32 x;
        F32 y;
        F32 z;
        F32 w;
    };
    struct
    {
        F32 r;
        F32 g;
        F32 b;
        F32 a;
    };
    F32 v[4];
};

/////////////////////////////////////////////////////// 
// NOTE(Sebas): Vector Helper Macros

#define V2_EXPANDED(v) (((v).x), ((v).y))
#define V3_EXPANDED(v) (((v).x), ((v).y), ((v).z))
#define V4_EXPANDED(v) (((v).x), ((v).y), ((v).z), ((v).w))

/////////////////////////////////////////////////////// 
// NOTE(Sebas): Vector Functions

function V2S32 v2s32(S32 x, S32 y);
function V2F32 v2f32(F32 x, F32 y);
function V3F32 v3f32(F32 x, F32 y, F32 z);
function V4F32 v4f32(F32 x, F32 y, F32 z, F32 w);

function V2S32 operator+(const V2S32& a, const V2S32& b);
function V2F32 operator+(const V2F32& a, const V2F32& b);
function V3F32 operator+(const V3F32& a, const V3F32& b);
function V4F32 operator+(const V4F32& a, const V4F32& b);

function V2S32 operator-(const V2S32& a, const V2S32& b);
function V2F32 operator-(const V2F32& a, const V2F32& b);
function V3F32 operator-(const V3F32& a, const V3F32& b);
function V4F32 operator-(const V4F32& a, const V4F32& b);

function B32 operator==(const V2S32& a, const V2S32& b);
function B32 operator==(const V2F32& a, const V2F32& b);
function B32 operator==(const V3F32& a, const V3F32& b);
function B32 operator==(const V4F32& a, const V4F32& b);

function V2S32 operator*(const V2S32& v, const S32& s);
function V2F32 operator*(const V2F32& v, const F32& s);
function V3F32 operator*(const V3F32& v, const F32& s);
function V4F32 operator*(const V4F32& v, const F32& s);

function V2S32 operator*(const S32& s, const V2S32& v);
function V2F32 operator*(const F32& s, const V2F32& v);
function V3F32 operator*(const F32& s, const V3F32& v);
function V4F32 operator*(const F32& s, const V4F32& v);

function V2F32 vec_hadamard(V2F32 a, V2F32 b);
function V3F32 vec_hadamard(V3F32 a, V3F32 b);
function V4F32 vec_hadamard(V4F32 a, V4F32 b);

function F32 vec_dot(V2F32 a, V2F32 b);
function F32 vec_dot(V3F32 a, V3F32 b);
function F32 vec_dot(V4F32 a, V4F32 b);