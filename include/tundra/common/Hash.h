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

#ifndef TUNDRA_HASH_H
#define TUNDRA_HASH_H

#include "tundra/common/TypeDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hashes an u64.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_u64(u64 i);

/**
 * @brief Hashes an i64.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_i64(i64 i);

/**
 * @brief Hashes an u32.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_u32(u32 i);

/**
 * @brief Hashes an int.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_int(int i);

/**
 * @brief Hashes a u16.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_u16(u16 i);

/**
 * @brief Hashes an i16.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_i16(i16 i);

/**
 * @brief Hashes an u8.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_u8(u8 i);

/**
 * @brief Hashes a i8.
 * 
 * @param i Integer to hash.
 * 
 * @return u64 Hash result. 
 */
u64 Tundra_hash_i8(i8 i);

#ifdef __cplusplus
}
#endif

#endif