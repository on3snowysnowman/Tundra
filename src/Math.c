/**
 * @file Math.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic math functions, currently only min, max and clamp.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/utils/Math.h"
#include "tundra/common/BitUtils.h"


// Clamp Implementations 
#define CLAMP_MIN_IMPL(name, type) \
type Tundra_clamp_min_##name(type num, type min) \
{ \
    return (num < min) ? min : num;  \
}
#define CLAMP_MAX_IMPL(name, type) \
type Tundra_clamp_max_##name(type num, type max) \
{ \
    return (num > max) ? max : num;  \
}

CLAMP_MIN_IMPL(u8, u8)
CLAMP_MAX_IMPL(u8, u8)
CLAMP_MIN_IMPL(i8, i8)
CLAMP_MAX_IMPL(i8, i8)
CLAMP_MIN_IMPL(u16, u16)
CLAMP_MAX_IMPL(u16, u16)
CLAMP_MIN_IMPL(i16, i16)
CLAMP_MAX_IMPL(i16, i16)
CLAMP_MIN_IMPL(u32, u32)
CLAMP_MAX_IMPL(u32, u32)  
CLAMP_MIN_IMPL(i32, i32)
CLAMP_MAX_IMPL(i32, i32)
CLAMP_MIN_IMPL(u64, u64)
CLAMP_MAX_IMPL(u64, u64)
CLAMP_MIN_IMPL(i64, i64)
CLAMP_MAX_IMPL(i64, i64)

u64 Tundra_ceil_pow2(u64 num)
{
    if(Tundra_is_pow2(num))
    {
        return num;
    }

    u8 msb_pos = Tundra_get_msb_pos(num);

    // Return the next power of two.
    return 1ULL << (msb_pos + 1);
}

u64 Tundra_uint_pow(u64 base, u8 exp)
{
    u64 result = 1;

    while (exp > 0)
    {
        if (IS_ODD(exp))   
            result *= base;

        base *= base;
        exp >>= 1; // exp /= 2
    }

    return result;
}