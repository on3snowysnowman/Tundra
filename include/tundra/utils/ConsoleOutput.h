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
#include "tundra/utils/CoreDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Outputs a single byte to standard output.
 * 
 * @param byte Byte to output.
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully written.
 */
i64 Tundra_stdout_byte(u8 byte);

/**
 * @brief Outputs a single char to standard output.
 * 
 * @param c Char to output.
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully written.
 */
i64 Tundra_stdout_char(char c);

/**
 * @brief Outputs a C String to standard output. Must scan the string to count 
 * the number of characters to output.
 * 
 * @param cstr String to output. 
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully written.
 */
i64 Tundra_stdout_cstr(const char *cstr);

/**
 * @brief Outputs a C String to standard output. `length` is the length of the 
 * string not including the null terminator.
 * 
 * @param cstr String to output.
 * @param length Length of the string, excluding null terminator.
 * 
 * @return Error code of write. Negative values are error codes, non negative 
 * values are the number of bytes successfully written.
 */
i64 Tundra_stdout_cstr_w_len(const char *cstr, u64 length);

i64 Tundra_stdout_u64(u64 num);

i64 Tundra_stdout_i64(i64 num);

i64 Tundra_stdout_u32(u32 num);

i64 Tundra_stdout_int(int num);

i64 Tundra_stdout_u16(u16 num);

i64 Tundra_stdout_i16(i16 num);

i64 Tundra_stdout_u8(u8 num);

i64 Tundra_stdout_i8(i8 num);

i64 Tundra_stdout_float(float num);


#ifdef __cplusplus
}   
#endif

#endif
