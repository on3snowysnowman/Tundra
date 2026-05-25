/**
 * @file ToString.h
 * @author your name (you@domain.com)
 * @brief Methods for converting numbers to c strings.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_TOSTRING_H
#define TUNDRA_TOSTRING_H

#include "tundra/utils/CoreDef.h"
#include "tundra/utils/MemAlloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TUNDRA_MAX_DIGITS_FOR_U64 20

/**
 * @brief Result of a ToString operation. Stores a C String and its length, 
 * which does not include the null terminator.
 * 
 * Caller is responsible for the allocated C String.
 */
typedef struct 
{
    char *str;
    u64 length;
} Tundra_ToStrResult;

/**
 * @brief Converts a u64 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_ToStrResult Conversion result.
 */
Tundra_ToStrResult Tundra_u64_to_str(u64 num);

/**
 * @brief Converts a u64 to a C string placing the results in the provided 
 * buffer.  
 * 
 * Ensure that the `output` buffer is at least TUNDRA_MAX_DIGITS_FOR_U64 + 1 
 * bytes long, +1 for the null terminator.  
 * 
 * @param num Num to convert.
 * @param output Output to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_u64_to_str_w_buf(u64 num, char *output);

#ifdef __cplusplus
}   
#endif

#endif