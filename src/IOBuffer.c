/**
 * @file IOBuffer.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Buffering input and output for both files and stdin and stdout before
 * they are written to disk / console. 
 * @version 0.1
 * @date 2026-05-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/internal/IOBuffer.h"
#include "tundra/utils/FatalHandler.h"

void InTundra_IOBuff_init(InTundra_IOBuffer *buff, InTundra_IOHandle handle)
{
    buff->cursor_pos = 0;
    buff->handle = handle;
    Tundra_DynArrU8_init(&buff->data);
}

void InTundra_IOBuff_free(InTundra_IOBuffer *buff)
{
    Tundra_DynArrU8_free(&buff->data);
}

void InTundra_IOBuff_write_bytes(InTundra_IOBuffer *buff, const u8 *bytes,
    u64 num_bytes)
{
    Tundra_DynArrU8_add_mult_copy(&buff->data, bytes, num_bytes);
}

i64 InTundra_IOBuff_flush(InTundra_IOBuffer *buff)
{        
    return InTundra_write_bytes(buff->handle, Tundra_DynArrU8_data(&buff->data), 
        (i64)Tundra_DynArrU8_size(&buff->data));
}
