/**
 * @file ConsoleIO.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic inputting and outputting with the Console.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_CONSOLEIO_H
#define TUNDRA_CONSOLEIO_H

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
 * @return Error code of write. Negative return values are error codes, 
 * non negative values are the number of bytes successfully outputted.
 */
i64 Tundra_print_byte(u8 byte);

/**
 * @brief Outputs a single char to standard output.
 * 
 * @param c Char to output.
 * 
 * @return Error code of write. Negative return values are error codes, 
 * non negative values are the number of bytes successfully outputted.
 */
i64 Tundra_print_char(char c);

/**
 * @brief Outputs a C String to standard output. Must scan the string to count 
 * the number of characters to output.
 * 
 * @param cstr String to output. 
 * 
 * @return Error code of write. Negative return values are error codes, 
 * non negative values are the number of bytes successfully outputted.
 */
i64 Tundra_print_cstr(const char *cstr);

/**
 * @brief Outputs a C String to standard output. `length` is the length of the 
 * string not including the null terminator.
 * 
 * @param cstr String to output.
 * @param length Length of the string, excluding null terminator.
 * 
 * @return Error code of write. Negative return values are error codes, 
 * non negative values are the number of bytes successfully outputted.
 */
i64 Tundra_print_cstr_w_len(const char *cstr, u64 length);

/**
 * @brief Outputs an u64 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_u64(u64 num);

/**
 * @brief Outputs an i64 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_i64(i64 num);

/**
 * @brief Outputs an u32 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_u32(u32 num);

/**
 * @brief Outputs an int to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_int(int num);

/**
 * @brief Outputs an u16 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_u16(u16 num);

/**
 * @brief Outputs an i16 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_i16(i16 num);

/**
 * @brief Outputs an u8 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_u8(u8 num);

/**
 * @brief Outputs an i8 to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_i8(i8 num);

/**
 * @brief Outputs a float to standard output.
 * 
 * @param num Num to output.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_float(float num);

/**
 * @brief Outputs a formatted message to standard output. Negative return values
 * are error codes, non negative values are the number of bytes successfully 
 * outputted.
 * 
 * @param format Formatted message to output
 * @param ... Arguments for the formatted message.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_print_fmt(const char *format, ...);

i64 Tundra_errprint_fmt(const char *format, ...);

/**
 * @brief Outputs a formatted message to standard output with an explicit VArgs
 * list. Negative return values are error codes, non negative values are the 
 * number of bytes successfully outputted.
 * 
 * @param format Formatted message to output
 * @param ... Arguments for the formatted message.
 * 
 * @return i64 Number of bytes printed if the return is non negative, otherwise 
 * it is an error code. 
 */
i64 Tundra_vargs_print_fmt(const char *format, Tundra_VaList args);

i64 Tundra_vargs_errprint_fmt(const char *format, Tundra_VaList args);

/**
 * @brief Reads a set number of bytes from standard input.
 * 
 * The read result is placed in `read_result_output`. Pass this as `NULL` to 
 * discard result. If the result is negative, it's an error code. Otherwise it 
 * is the number of bytes successfully read.
 * 
 * @param handle Handle to read from.
 * @param output Output buffer to place read bytes into.
 * @param num_bytes Number of bytes to read.
 * @param read_result_output Output variable to place read result in. Pass as 
 * `NULL` to discard result.
 */
void Tundra_readin_bytes(void *output, i64 num_bytes, i64 *read_result_output);

/**
 * @brief Reads a char from standard input.
 * 
 * The read result is placed in `read_result_output`. Leave this as `NULL` to
 * discard result. If the result is negative, it's an error code. Otherwise it 
 * is the number of bytes successfully read. 
 * 
 * @param read_result_output Output variable to place read result in. Pass as 
 * `NULL` to discard result.
 * 
 * @return char Read character, or -1 if read failed.
 */
char Tundra_readin_char(i64 *read_result_output);

void Tundra_flush_stdout(void);

void InTundra_ConIO_init(void);

void InTundra_ConIO_shutdown(void);

#ifdef __cplusplus
}   
#endif

#endif
