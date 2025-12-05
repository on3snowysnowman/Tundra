/**
 * @file Math.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic math functions, currently only min, max and Tundra_clamp.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_MATH_H
#define TUNDRA_MATH_H

#include "tundra/utils/CoreDef.h"

#ifdef __cplusplus
extern "C" {    
#endif

uint8 Tundra_clamp_min_u8(uint8 num, uint8 min);
uint8 Tundra_clamp_max_u8(uint8 num, uint8 max);
uint16 Tundra_clamp_min_u16(uint16 num, uint16 min);
uint16 Tundra_clamp_max_u16(uint16 num, uint16 max);
uint32 Tundra_clamp_min_u32(uint32 num, uint32 min);
uint32 Tundra_clamp_max_u32(uint32 num, uint32 max);
uint64 Tundra_clamp_min_u64(uint64 num, uint64 min);
uint64 Tundra_clamp_max_u64(uint64 num, uint64 max);   

#ifdef __cplusplus
} // extern "C" 
#endif

#endif