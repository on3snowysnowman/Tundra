/**
 * @file IOBuffer.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Buffering input and output for both files and stdin and stdout before
 * they are written to disk / console. 
 * @version 0.1
 * @date 2026-05-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_IOBUFFER_H
#define TUNDRA_IOBUFFER_H

#include "tundra/internal/IOInterface.h"
#include "tundra/containers/DynamicArrayU8.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Buffer for input and output. Can be used to delay writes to or
 * reads from handles.
 */
typedef struct
{
    InTundra_IOHandle handle;
    Tundra_DynamicArrayU8 data;
    i64 cursor_pos;    
} InTundra_IOBuffer;

/**
 * @brief Initializes a buffer. Init required before use.
 * 
 * @param buff Buffer to initialize.
 * @param handle Handle the buffer interacts with.
 */
void InTundra_IOBuff_init(InTundra_IOBuffer *buff, InTundra_IOHandle handle);

/**
 * @brief Frees a buffer. Required when buffer is no longer needed.
 * 
 * @param buff Buffer to free.
 */
void InTundra_IOBuff_free(InTundra_IOBuffer *buff);

/**
 * @brief Writes bytes to the buffer, flushing if the buffer becomes overfilled.
 * Returns the number of bytes written to buffer.
 * 
 * @param buff Buffer to write to.
 * @param bytes Bytes to write.
 * @param num_bytes Number of bytes to write from `bytes`.
 * 
 * @return i64 Number of bytes written
 */
i64 InTundra_IOBuff_write_bytes(InTundra_IOBuffer *buff, const u8 *bytes,
    u64 num_bytes);
    
/**
 * @brief Reads bytes from the buffer, fetching more bytes into the buffer if
 * there is not enough to satisfy the request. Returns number of bytes read
 * into `bytes`.
 * 
 * @param buff Buffer to read bytes from.
 * @param bytes Bytes buffer to read bytes into.
 * @param num_bytes Number of bytes to read.
 * 
 * @return i64 Number of bytes read
 */
i64 InTundra_IOBuff_read_bytes(InTundra_IOBuffer *buff, u8 *bytes, 
    u64 num_bytes);

u64 InTundra_IOBuff_size(InTundra_IOBuffer *buff);

char InTundra_IOBuff_peek(InTundra_IOBuffer *buff);

/**
 * @brief Writes the buffer to its registered handle. If the return is negative
 * it's an error code. Otherwise it's the number of bytes written.
 * 
 * @param buff Buffer to write.
 * 
 * @return i64 Error code if negative, otherwise number of bytes written. 
 */
i64 InTundra_IOBuff_flush(InTundra_IOBuffer *buff);
 

// #include "tundra/utils/ConsoleIO.h"
// static inline void print_buff(InTundra_IOBuffer *buff)
// {
//     const u64 buff_size = Tundra_DynArrU8_size(&buff->data);
//     const u64 buff_cap = Tundra_DynArrU8_capacity(&buff->data);

//     Tundra_print_fmt("Buffer: %d/%d\n", (int)buff_size, (int)buff_cap);

//     Tundra_print_char('[');

//     for(u64 i = 0; i < buff_size; ++i)
//     {
//         char c = (char)*Tundra_DynArrU8_at(&buff->data, i);

//         if(c == '\n')
//         {
//             Tundra_print_char('N');
//             continue;
//         }

//         Tundra_print_char((char)*Tundra_DynArrU8_at(&buff->data, i));
//     }

//     Tundra_print_cstr("]\n\n");
// }

#ifdef __cplusplus
}
#endif

#endif
