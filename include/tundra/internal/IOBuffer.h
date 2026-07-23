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

i64 InTundra_OBuff_write_bytes(InTundra_OutputBuffer *buff, const u8 *bytes,
    u64 num_bytes);

i64 InTundra_OBuff_write_char(InTundra_OutputBuffer *buff, char c);

i64 InTundra_OBuff_write_u64(InTundra_OutputBuffer *buff, u64 num);

i64 InTundra_OBuff_write_i64(InTundra_OutputBuffer *buff, i64 num);

i64 InTundra_OBuff_write_u32(InTundra_OutputBuffer *buff, u32 num);

i64 InTundra_OBuff_write_int(InTundra_OutputBuffer *buff, int num);

i64 InTundra_OBuff_write_u16(InTundra_OutputBuffer *buff, u16 num);

i64 InTundra_OBuff_write_i16(InTundra_OutputBuffer *buff, i16 num);

i64 InTundra_OBuff_write_u8(InTundra_OutputBuffer *buff, u8 num);

i64 InTundra_OBuff_write_i8(InTundra_OutputBuffer *buff, i8 num);

i64 InTundra_OBuff_write_float(InTundra_OutputBuffer *buff, float num);

void InTundra_IBuff_readin_bytes(InTundra_InputBuffer *buff, u8 *bytes, 
    u64 num_bytes, i64 *read_result_output); 

char InTundra_IBuff_readin_char(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

u64 InTundra_IBuff_readin_u64(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

i64 InTundra_IBuff_readin_i64(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

u32 InTundra_IBuff_readin_u32(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

int InTundra_IBuff_readin_int(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

u16 InTundra_IBuff_readin_u16(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

i16 InTundra_IBuff_readin_i16(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

u8 InTundra_IBuff_readin_u8(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

i8 InTundra_IBuff_readin_i8(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

float InTundra_IBuff_readin_float(InTundra_InputBuffer *buff, 
    i64 *read_result_output);

i64 InTundra_OBuff_flush(InTundra_OutputBuffer *buff);

i64 InTundra_IBuff_peek(InTundra_InputBuffer *buff);

u64 InTundra_OBuff_size(InTundra_OutputBuffer *buff);

u64 InTundra_IBuff_size(InTundra_InputBuffer *buff);

void InTundra_IBuff_clear(InTundra_InputBuffer *buff);

void InTundra_IBuff_discard(InTundra_InputBuffer *buff, u64 num_bytes);

#ifdef __cplusplus
}
#endif

#endif
