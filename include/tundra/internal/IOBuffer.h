/**
 * @file IOBuffer.h
 * @author your name (you@domain.com)
 * @brief Provides buffering for input and output from primitive handles.
 * @version 0.1
 * @date 2026-07-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef TUNDRA_IOBUFFER_H
#define TUNDRA_IOBUFFER_H

#include "tundra/internal/IOInterface.h"
#include "tundra/containers/DynamicStackU8.h"
#include "tundra/containers/DynamicArrayU8.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    InTundra_IOHandle handle;
    Tundra_DynamicStackU8 data;
} InTundra_InputBuffer;

typedef struct
{
    InTundra_IOHandle handle;
    Tundra_DynamicArrayU8 data;
} InTundra_OutputBuffer;

void InTundra_IBuff_init(InTundra_InputBuffer *buff, InTundra_IOHandle handle);

void InTundra_OBuff_init(InTundra_OutputBuffer *buff, 
    InTundra_IOHandle handle);

void InTundra_IBuff_free(InTundra_InputBuffer *buff);

void InTundra_OBuff_free(InTundra_OutputBuffer *buff);

i64 InTundra_OBuff_write(InTundra_OutputBuffer *buff, const u8 *bytes,
    u64 num_bytes);

i64 InTundra_IBuff_read(InTundra_InputBuffer *buff, u8 *bytes, u64 num_bytes); 

i64 InTundra_OBuff_flush(InTundra_OutputBuffer *buff);

i64 InTundra_IBuff_peek(InTundra_InputBuffer *buff);

void InTundra_IBuff_clear(InTundra_InputBuffer *buff);

void InTundra_IBuff_discard(InTundra_InputBuffer *buff, u64 num_bytes);

#ifdef __cplusplus
}
#endif

#endif
