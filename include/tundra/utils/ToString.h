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

#define TUNDRA_MAX_CHARS_TO_REPRESENT_U8 3
#define TUNDRA_MAX_CHARS_TO_REPRESENT_I8 4
#define TUNDRA_MAX_CHARS_TO_REPRESENT_U16 5
#define TUNDRA_MAX_CHARS_TO_REPRESENT_I16 6
#define TUNDRA_MAX_CHARS_TO_REPRESENT_U32 10
#define TUNDRA_MAX_CHARS_TO_REPRESENT_I32 11
#define TUNDRA_MAX_CHARS_TO_REPRESENT_U64 20
#define TUNDRA_MAX_CHARS_TO_REPRESENT_I64 20


u64 InTundra_int_to_str_helper(u64 num, char *buffer, char *output, 
    bool negative_num);

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

Tundra_ToStrResult Tundra_i64_to_str(i64 num);

Tundra_ToStrResult Tundra_u32_to_str(u32 num);

Tundra_ToStrResult Tundra_int_to_str(int num);

Tundra_ToStrResult Tundra_u16_to_str(u16 num);

Tundra_ToStrResult Tundra_i16_to_str(i16 num);

Tundra_ToStrResult Tundra_u8_to_str(u8 num);

Tundra_ToStrResult Tundra_i8_to_str(i8 num);


/**
 * @brief Converts a u64 to a C string placing the results in the provided 
 * buffer.  
 * 
 * Ensure that the `output` buffer is at least TUNDRA_MAX_CHARS_TO_REPRESENT_U64 + 1 
 * bytes long, +1 for the null terminator.  
 * 
 * @param num Num to convert.
 * @param output Output to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_u64_to_str_buf(u64 num, char *output);

u64 Tundra_i64_to_str_buf(i64 num, char *output);

u64 Tundra_u32_to_str_buf(u32 num, char *output);

u64 Tundra_int_to_str_buf(int num, char *output);

u64 Tundra_u16_to_str_buf(u16 num, char *output);

u64 Tundra_i16_to_str_buf(i16 num, char *output);

u64 Tundra_u8_to_str_buf(u8 num, char *output);

u64 Tundra_i8_to_str_buf(i8 num, char *output);


#ifdef __cplusplus
}   
#endif

#endif