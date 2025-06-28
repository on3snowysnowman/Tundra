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

#ifndef TUNDRA_HGUARD_HASH_H
#define TUNDRA_HGUARD_HASH_H


#include <stdint.h>

/**
 * @brief Computes a hash value for a uint64_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_uint64_t(uint64_t i)
{
    i ^= i >> 33;
    i *= 0xff51afd7ed558ccdULL;
    i ^= i >> 33;
    i *= 0xc4ceb9fe1a85ec53ULL;
    i ^= i >> 33;
    return i;
}

/**
 * @brief Computes a hash value for a int64_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_int64(int64_t i)
{
    return Tundra_hash_uint64_t((uint64_t)(i));
}

/**
 * @brief Computes a hash value for a uint32_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_uint32(uint32_t i)
{
    return Tundra_hash_uint64_t((uint64_t)(i));
}

/**
 * @brief Computes a hash value for a int32_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_int32(int32_t i)
{
    return Tundra_hash_uint64_t((uint64_t)(uint32_t)(i));
}

/**
 * @brief Computes a hash value for a uint16_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_uint16(uint16_t i)
{
    return Tundra_hash_uint64_t((uint64_t)(i));
}

/**
 * @brief Computes a hash value for a int16_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_int16(int16_t i)
{
    return Tundra_hash_uint64_t((uint64_t)(uint16_t)(i));
}

/**
 * @brief Computes a hash value for a uint8_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_uint8(uint8_t i)
{
    return Tundra_hash_uint64_t((uint64_t)(i));
}

/**
 * @brief Computes a hash value for a int8_t integer.
 * 
 * @param i The integer to hash.
 */
static inline uint64_t Tundra_hash_int8(int8_t i)
{
    return Tundra_hash_uint64_t((uint64_t)(uint8_t)(i));
}

#endif // TUNDRA_HGUARD_HASH_H