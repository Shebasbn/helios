////////////////////////////////
// NOTE(sebas): Vector Functions

#pragma warning(push)
#pragma warning(disable : 5246)

function V2S32 
v2s32(S32 x, S32 y)
{
    V2S32 r = { x, y };
    return r;
}

function V2F32 
v2f32(F32 x, F32 y)
{
    V2F32 r = { x, y };
    return r;
}

function V3F32 
v3f32(F32 x, F32 y, F32 z)
{
    V3F32 r = { x, y, z };
    return r;
}

function V4F32 
v4f32(F32 x, F32 y, F32 z, F32 w)
{
    V4F32 r = { x, y, z, w};
    return r;
}

function V2S32 
operator+(const V2S32& a, const V2S32& b)
{
    V2S32 r = { a.x + b.x, a.y + b.y };
    return r;
}

function V2F32 
operator+(const V2F32& a, const V2F32& b)
{
    V2F32 r = { a.x + b.x, a.y + b.y };
    return r;
}
function V3F32 
operator+(const V3F32& a, const V3F32& b)
{
    V3F32 r = { a.x + b.x, a.y + b.y, a.z + b.z };
    return r;
}
function V4F32 
operator+(const V4F32& a, const V4F32& b)
{
    V4F32 r = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return r;
}

function V2S32 
operator-(const V2S32& a, const V2S32& b)
{
    V2S32 r = { a.x - b.x, a.y - b.y };
    return r;
}

function V2F32 
operator-(const V2F32& a, const V2F32& b)
{
    V2F32 r = { a.x - b.x, a.y - b.y };
    return r;
}
function V3F32 
operator-(const V3F32& a, const V3F32& b)
{
    V3F32 r = { a.x - b.x, a.y - b.y, a.z - b.z };
    return r;
}
function V4F32 
operator-(const V4F32& a, const V4F32& b)
{
    V4F32 r = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    return r;
}

function B32 
operator==(const V2S32& a, const V2S32& b)
{
    B32 result = (a.x == b.x && a.y == b.y);
    return result;
}

function B32 
operator==(const V2F32& a, const V2F32& b)
{
    B32 result = (a.x == b.x && a.y == b.y);
    return result;
}

function B32 
operator==(const V3F32& a, const V3F32& b)
{
    B32 result = (a.x == b.x && a.y == b.y && a.z == b.z);
    return result;
}

function B32 
operator==(const V4F32& a, const V4F32& b)
{
    B32 result = (a.x == b.x && a.y == b.y && a.z == b.z && a.w == a.w);
    return result;
}

function V2S32 
operator*(const V2S32& v, const S32& s)
{
    V2S32 r = { v.x * s, v.y * s };
    return r;
}
function V2F32 
operator*(const V2F32& v, const F32& s)
{
    V2F32 r = { v.x * s, v.y * s };
    return r;
}
function V3F32 
operator*(const V3F32& v, const F32& s)
{
    V3F32 r = { v.x * s, v.y * s, v.z * s };
    return r;
}
function V4F32 
operator*(const V4F32& v, const F32& s)
{
    V4F32 r = { v.x * s, v.y * s, v.z * s, v.w * s };
    return r;
}

function V2S32 
operator*(const S32& s, const V2S32& v)
{
    V2S32 r = { v.x * s, v.y * s };
    return r;
}
function V2F32 
operator*(const F32& s, const V2F32& v)
{
    V2F32 r = { v.x * s, v.y * s };
    return r;
}
function V3F32 
operator*(const F32& s, const V3F32& v)
{
    V3F32 r = { v.x * s, v.y * s, v.z * s };
    return r;
}
function V4F32 
operator*(const F32& s, const V4F32& v)
{
    V4F32 r = { v.x * s, v.y * s, v.z * s, v.w * s };
    return r;
}

function V2F32 
vec_hadamard(V2F32 a, V2F32 b)
{
    V2F32 r = { a.x * b.x, b.y * b.y };
    return r;
}

function V3F32 
vec_hadamard(V3F32 a, V3F32 b)
{
    V3F32 r = { a.x * b.x, a.y * b.y, a.z * b.z };
    return r;
}

function V4F32 
vec_hadamard(V4F32 a, V4F32 b)
{
    V4F32 r = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
    return r;
}

function F32 
vec_dot(V2F32 a, V2F32 b)
{
    F32 r = a.x * b.x + a.y * b.y;
    return r;
}
function F32 
vec_dot(V3F32 a, V3F32 b)
{
    F32 r = a.x * b.x + a.y * b.y + a.z * b.z;
    return r;
}
function F32 
vec_dot(V4F32 a, V4F32 b)
{
    F32 r = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return r;
}

#pragma warning(pop)