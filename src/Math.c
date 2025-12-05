/**
 * @file Math.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic math functions, currently only min, max and clamp.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#include "tundra/utils/Math.h"

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

CLAMP_MIN_IMPL(u8, uint8)
CLAMP_MAX_IMPL(u8, uint8)
CLAMP_MIN_IMPL(i8, int8)
CLAMP_MAX_IMPL(i8, int8)
CLAMP_MIN_IMPL(u16, uint16)
CLAMP_MAX_IMPL(u16, uint16)
CLAMP_MIN_IMPL(i16, int16)
CLAMP_MAX_IMPL(i16, int16)
CLAMP_MIN_IMPL(u32, uint32)
CLAMP_MAX_IMPL(u32, uint32)  
CLAMP_MIN_IMPL(i32, int32)
CLAMP_MAX_IMPL(i32, int32)
CLAMP_MIN_IMPL(u64, uint64)
CLAMP_MAX_IMPL(u64, uint64)
CLAMP_MIN_IMPL(i64, int64)
CLAMP_MAX_IMPL(i64, int64)

