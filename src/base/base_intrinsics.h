#pragma once

#if INTRINSICS_MICROSOFT
# include <intrin.h>
# define intrinsic_rdtsc() __rdtsc()
# define intrinsic_debug_break() __debugbreak()
#else
# error Current Intrinsics not supported!
#endif