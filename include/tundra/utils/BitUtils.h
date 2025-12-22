/**
 * @file BitUtils.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for reading certain properties of bits, such as leading zeros.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
 */

#ifndef TUNDRA_BITUTILS_H
#define TUNDRA_BITUTILS_H

#include "tundra/utils/CoreDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TUNDRA_KIBIBYTE 2ULL << 9
#define TUNDRA_MEBIBYTE 2ULL << 19
#define TUNDRA_GIBIBYTE 2ULL << 29

static inline uint8 Tundra_get_num_lead_zeros(uint64 bits)
{
    return __builtin_clzll(bits);
}

static inline uint8 Tundra_get_num_trail_zeros(uint64 bits)
{
    return __builtin_ctzll(bits);
}

static inline uint8 Tundra_get_msb_pos(uint64 bits)
{
    return 63ULL - Tundra_get_num_lead_zeros(bits);
}

static inline uint8 Tundra_get_lsb_pos(uint64 bits)
{
    return Tundra_get_num_trail_zeros(bits);
}

/**
 * @brief Returns whether `num` is a power of two.
 * 
 * If `num` is zero, false is returned.
 * 
 * @param num Number to check.
 * 
 * @return true if `num` is a power of two, false otherwise.
 */
static inline bool Tundra_is_pow2(uint64 num)
{
    return (num != 0) && (num & (num - 1)) == 0;
}

#ifdef __cplusplus
}   
#endif

#endif
