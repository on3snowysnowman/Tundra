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
int8 Tundra_clamp_min_i8(int8 num, int8 min);
int8 Tundra_clamp_max_i8(int8 num, int8 max);
uint16 Tundra_clamp_min_u16(uint16 num, uint16 min);
uint16 Tundra_clamp_max_u16(uint16 num, uint16 max);
int16 Tundra_clamp_min_i16(int16 num, int16 min);
int16 Tundra_clamp_max_i16(int16 num, int16 max);
uint32 Tundra_clamp_min_u32(uint32 num, uint32 min);
uint32 Tundra_clamp_max_u32(uint32 num, uint32 max);
int32 Tundra_clamp_min_i32(int32 num, int32 min);
int32 Tundra_clamp_max_i32(int32 num, int32 max);
uint64 Tundra_clamp_min_u64(uint64 num, uint64 min);
uint64 Tundra_clamp_max_u64(uint64 num, uint64 max);   
int64 Tundra_clamp_min_i64(int64 num, int64 min);
int64 Tundra_clamp_max_i64(int64 num, int64 max);

/** 
 * @brief Returns the smallest power of two greater than or equal to `num`.
 * 
 * `num` must be greater than 0.
 * 
 * @param num Number to ceil to next power of two.
 * 
 * @return uint64 Smallest power of two greater than or equal to `num`.
 */
uint64 Tundra_ceil_pow2(uint64 num);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif