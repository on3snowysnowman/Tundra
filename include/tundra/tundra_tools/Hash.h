/**
 * @file Hash.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Hash function for general int types.
 * @version 0.1
 * @date 06-26-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef HASH_H
#define HASH_H

#include <stdint.h>

/**
 * @brief Computes a hash value for a any size integer, signed or unsigned.
 * 
 * Users should use the `HASH_INT(i)` type-generic macro to handle proper integer
 * upscaling/signed conversions.  
 * 
 * @param i The 32-bit integer to hash.
 */
static inline uint64_t hash_int(uint64_t i)
{
    i ^= i >> 33;
    i *= 0xff51afd7ed558ccdULL;
    i ^= i >> 33;
    i *= 0xc4ceb9fe1a85ec53ULL;
    i ^= i >> 33;
    return i;
}


#define HASH_INT(i) _Generic((i), \
    int8_t:   hash_int((uint64_t)(uint8_t)(i)), \
    uint8_t:  hash_int((uint64_t)(i)), \
    int16_t:  hash_int((uint64_t)(uint16_t)(i)), \
    uint16_t: hash_int((uint64_t)(i)), \
    int32_t:  hash_int((uint64_t)(uint32_t)(i)), \
    uint32_t: hash_int((uint64_t)(i)), \
    int64_t:  hash_int((uint64_t)(i)), \
    uint64_t: hash_int((uint64_t)(i)), \
    default:  hash_int((uint64_t)(i)) \
)

#endif // HASH_H