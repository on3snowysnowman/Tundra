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

typedef struct
{
    InTundra_IOHandle handle;
    Tundra_DynamicArrayU8 data;
    i64 cursor_pos;    
} InTundra_IOBuffer;

void InTundra_IOBuff_init(InTundra_IOBuffer *buff, InTundra_IOHandle handle);

void InTundra_IOBuff_free(InTundra_IOBuffer *buff);

void InTundra_IOBuff_write_bytes(InTundra_IOBuffer *buff, const u8 *bytes,
    u64 num_bytes);
    
i64 InTundra_IOBuff_flush(InTundra_IOBuffer *buff);

#ifdef __cplusplus
}
#endif

#endif
