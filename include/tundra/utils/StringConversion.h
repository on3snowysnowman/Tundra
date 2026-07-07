/**
 * @file StringConversion.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for converting numbers to C strings and C strings to numbers. 
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_STRINGCONVERSION_H
#define TUNDRA_STRINGCONVERSION_H

#include "tundra/common/TypeDef.h"
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

#define TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT_INT_PART 39
#define TUNDRA_FLOAT_PRECISION 6

// Int part + decimal digit + fractional part
#define TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT \
    TUNDRA_MAX_CHARS_TO_REPRESENT_FLOAT_INT_PART + 1 + TUNDRA_FLOAT_PRECISION

#define INTUNDRA_FLOAT_ROUNDING_CONSTANT 0.0005f

// u64 InTundra_int_to_cstr_helper(u64 num, char *buffer, char *output, 
//     bool negative_num);

/**
 * @brief Converts an u64 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_u64_to_cstr(u64 num);

/**
 * @brief Converts an i64 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_i64_to_cstr(i64 num);

/**
 * @brief Converts an u32 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_u32_to_cstr(u32 num);

/**
 * @brief Converts an int to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_int_to_cstr(int num);

/**
 * @brief Converts an u16 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_u16_to_cstr(u16 num);

/**
 * @brief Converts an i16 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_i16_to_cstr(i16 num);

/**
 * @brief Converts an u8 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_u8_to_cstr(u8 num);

/**
 * @brief Converts an i8 to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_i8_to_cstr(i8 num);

/**
 * @brief Converts a float to a heap allocated C string . Caller manages the 
 * allocated memory.
 * 
 * @param num Num to convert.
 * 
 * @return Tundra_CStr Heap allocated CStr, caller is responsible for memory.
 */
Tundra_CStr Tundra_float_to_cstr(float num);

/**
 * @brief Converts an u64 to a C string placing the results in the provided 
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
u64 Tundra_u64_to_cstr_buf(u64 num, char *output);

/**
 * @brief Converts an i64 to a C string placing the results in the provided 
 * buffer and returning the length of the converted string.
 * 
 * Ensure that the `output` buffer is at least 
 * `TUNDRA_MAX_CHARS_TO_REPRESENT_I64 + 1` bytes long, the + 1 is for the null 
 * terminator.  
 * 
 * @param num Num to convert.
 * @param output Buffer to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_i64_to_cstr_buf(i64 num, char *output);

/**
 * @brief Converts an u32 to a C string placing the results in the provided 
 * buffer and returning the length of the converted string.
 * 
 * Ensure that the `output` buffer is at least 
 * `TUNDRA_MAX_CHARS_TO_REPRESENT_U32 + 1` bytes long, the + 1 is for the null 
 * terminator.  
 * 
 * @param num Num to convert.
 * @param output Buffer to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_u32_to_cstr_buf(u32 num, char *output);

/**
 * @brief Converts an int to a C string placing the results in the provided 
 * buffer and returning the length of the converted string.
 * 
 * Ensure that the `output` buffer is at least 
 * `TUNDRA_MAX_CHARS_TO_REPRESENT_I32 + 1` bytes long, the + 1 is for the null 
 * terminator.  
 * 
 * @param num Num to convert.
 * @param output Buffer to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_int_to_cstr_buf(int num, char *output);

/**
 * @brief Converts an u16 to a C string placing the results in the provided 
 * buffer and returning the length of the converted string.
 * 
 * Ensure that the `output` buffer is at least 
 * `TUNDRA_MAX_CHARS_TO_REPRESENT_U16 + 1` bytes long, the + 1 is for the null 
 * terminator.  
 * 
 * @param num Num to convert.
 * @param output Buffer to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_u16_to_cstr_buf(u16 num, char *output);

/**
 * @brief Converts an i16 to a C string placing the results in the provided 
 * buffer and returning the length of the converted string.
 * 
 * Ensure that the `output` buffer is at least 
 * `TUNDRA_MAX_CHARS_TO_REPRESENT_i16 + 1` bytes long, the + 1 is for the null 
 * terminator.  
 * 
 * @param num Num to convert.
 * @param output Buffer to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_i16_to_cstr_buf(i16 num, char *output);

/**
 * @brief Converts an u8 to a C string placing the results in the provided 
 * buffer and returning the length of the converted string.
 * 
 * Ensure that the `output` buffer is at least 
 * `TUNDRA_MAX_CHARS_TO_REPRESENT_U8 + 1` bytes long, the + 1 is for the null 
 * terminator.  
 * 
 * @param num Num to convert.
 * @param output Buffer to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_u8_to_cstr_buf(u8 num, char *output);

/**
 * @brief Converts an I8 to a C string placing the results in the provided 
 * buffer and returning the length of the converted string.
 * 
 * Ensure that the `output` buffer is at least 
 * `TUNDRA_MAX_CHARS_TO_REPRESENT_I8 + 1` bytes long, the + 1 is for the null 
 * terminator.  
 * 
 * @param num Num to convert.
 * @param output Buffer to place converted characters into. 
 * 
 * @return Number of characters in the converted string, not including the 
 * null terminator. If return is 0, conversion failed.
 */
u64 Tundra_i8_to_cstr_buf(i8 num, char *output);

u64 Tundra_float_to_cstr_buf(float num, char *output);

u8 Tundra_str_to_u8(const char *str);

i8 Tundra_str_to_i8(const char *str);

u16 Tundra_str_to_u16(const char *str);

i16 Tundra_str_to_i16(const char *str);

u32 Tundra_str_to_u32(const char *str);

int Tundra_str_to_int(const char *str);

u64 Tundra_str_to_u64(const char *str);

i64 Tundra_str_to_i64(const char *str);

float Tundra_str_to_float(const char *str);

#ifdef __cplusplus
}   
#endif

#endif
