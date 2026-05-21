#pragma once

#include "os/os_abs.h"
#include "os/os_input.h"

#if OS_WINDOWS
# include "os/win32/win32_helios.h"
#else
# error no backend for os_inc.h on this operating system
#endif