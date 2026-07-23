/**
 * @file Hash.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Hash methods for integral types
 * @version 0.1
 * @date 2026-07-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/common/Hash.h"

u64 Tundra_hash_u64(u64 i)
{
    i ^= i >> 33;
    i *= 0xff51afd7ed558ccdULL;
    i ^= i >> 33;
    i *= 0xc4ceb9fe1a85ec53ULL;
    i ^= i >> 33;
    return i;
}

#define HASH_IMPL(type) \
u64 Tundra_hash_##type(type i) { \
    return Tundra_hash_u64((u64)i); }

HASH_IMPL(i64)
HASH_IMPL(u32)
HASH_IMPL(int)
HASH_IMPL(u16)
HASH_IMPL(i16)
HASH_IMPL(u8)
HASH_IMPL(i8)
