/**
 * @file ConsoleOutput.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic outputting to the Console.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_IO_H
#define TUNDRA_IO_H

#include "tundra/internal/IOInterface.h"
#include "tundra/common/TypeDef.h"
#include "tundra/common/VariadicArgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Outputs a single byte to standard output.
 * 
 * @param byte Byte to output.
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully outputted.
 */
i64 Tundra_print_byte(u8 byte);

/**
 * @brief Outputs a single char to standard output.
 * 
 * @param c Char to output.
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully outputted.
 */
i64 Tundra_print_char(char c);

/**
 * @brief Outputs a C String to standard output. Must scan the string to count 
 * the number of characters to output.
 * 
 * @param cstr String to output. 
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully outputted.
 */
i64 Tundra_print_cstr(const char *cstr);

/**
 * @brief Outputs a C String to standard output. `length` is the length of the 
 * string not including the null terminator.
 * 
 * @param cstr String to output.
 * @param length Length of the string, excluding null terminator.
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully outputted.
 */
i64 Tundra_print_cstr_w_len(const char *cstr, u64 length);

/**
 * @brief Outputs an u64 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_u64(u64 num);

/**
 * @brief Outputs an i64 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_i64(i64 num);

/**
 * @brief Outputs an u32 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_u32(u32 num);

/**
 * @brief Outputs an int to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_int(int num);

/**
 * @brief Outputs an u16 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_u16(u16 num);

/**
 * @brief Outputs an i16 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_i16(i16 num);

/**
 * @brief Outputs an u8 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_u8(u8 num);

/**
 * @brief Outputs an i8 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_i8(i8 num);

/**
 * @brief Outputs a float to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Error code of the write. Negative values are error codes, non
 * negative values are the number of bytes successfully outputted. 
 */
i64 Tundra_print_float(float num);

i64 Tundra_printf(const char *format, ...);

i64 Tundra_vargs_printf(const char *format, Tundra_VaList args);

#ifdef __cplusplus
}   
#endif

#endif
