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

#include "tundra/common/TypeDef.h"

#define IS_POW_TWO(x) ((x & (x - 1)) == 0)
#define IS_ODD(x) (x & 1)
#define IS_EVEN(x) (!(x & 1))

#ifdef __cplusplus
extern "C" {    
#endif

u8 Tundra_clamp_min_u8(u8 num, u8 min);
u8 Tundra_clamp_max_u8(u8 num, u8 max);
i8 Tundra_clamp_min_i8(i8 num, i8 min);
i8 Tundra_clamp_max_i8(i8 num, i8 max);
u16 Tundra_clamp_min_u16(u16 num, u16 min);
u16 Tundra_clamp_max_u16(u16 num, u16 max);
i16 Tundra_clamp_min_i16(i16 num, i16 min);
i16 Tundra_clamp_max_i16(i16 num, i16 max);
u32 Tundra_clamp_min_u32(u32 num, u32 min);
u32 Tundra_clamp_max_u32(u32 num, u32 max);
i32 Tundra_clamp_min_i32(i32 num, i32 min);
i32 Tundra_clamp_max_i32(i32 num, i32 max);
u64 Tundra_clamp_min_u64(u64 num, u64 min);
u64 Tundra_clamp_max_u64(u64 num, u64 max);   
i64 Tundra_clamp_min_i64(i64 num, i64 min);
i64 Tundra_clamp_max_i64(i64 num, i64 max);

/** 
 * @brief Returns the smallest power of two greater than or equal to `num`.
 * 
 * `num` must be greater than 0.
 * 
 * @param num Number to ceil to next power of two.
 * 
 * @return u64 Smallest power of two greater than or equal to `num`.
 */
u64 Tundra_ceil_pow2(u64 num);

u64 Tundra_uint_pow(u64 base, u8 exp);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif