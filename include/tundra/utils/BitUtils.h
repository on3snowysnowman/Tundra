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

#endif
