#include <common.h>

#include "helios.h"

#include <stdio.h>

struct foo
{
    int a, b, c;
};

struct bar
{
    int a, b, c, d;
};

int main()
{
    foo f = { 0, 1, 2 };
    bar b;
    foo f2;
    M_ZeroStruct(&b);
    Assert(sizeof(f) > 0);
    M_CopyStruct(&f2, &f);
    
    foo foo_arr[10];
    M_ZeroArray(foo_arr);
    \
    foo foo2_arr[14];
    M_CopyArray(foo2_arr, foo_arr);
    
    
    return 0;
}