/**
 * @file Math.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic math functions, currently only min, max and clamp.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#include "tundra/utils/Math.h"

#define CLAMP_MIN_IMPL(type) \
type Tundra_clamp_min_##type(type num, type min) \
{ \
    return (num < min) ? min : num;  \
}
#define CLAMP_MAX_IMPL(type) \
type Tundra_clamp_max_##type(type num, type max) \
{ \
    return (num > max) ? max : num;  \
}

CLAMP_MIN_IMPL(uint8)
CLAMP_MAX_IMPL(uint8)
CLAMP_MIN_IMPL(uint16)
CLAMP_MAX_IMPL(uint16)
CLAMP_MIN_IMPL(uint32)
CLAMP_MAX_IMPL(uint32)  
CLAMP_MIN_IMPL(uint64)
CLAMP_MAX_IMPL(uint64)
