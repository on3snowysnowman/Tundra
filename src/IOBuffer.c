/**
 * @file IOBuffer.c
 * @author your name (you@domain.com)
 * @brief Provides buffering for input and output from primitive handles.
 * @version 0.1
 * @date 2026-07-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/internal/IOBuffer.h"
#include "tundra/common/Core.h"


#define DEF_BUFF_SIZE 32U

TUNDRA_CT_ASSERT(IS_POW_TWO(DEF_BUFF_SIZE));

void InTundra_IBuff_init(InTundra_InputBuffer *buff, InTundra_IOHandle handle)
{
    buff->handle = handle;
    Tundra_DynStkU8_init(&buff->data);
}

void InTundra_OBuff_init(InTundra_OutputBuffer *buff, 
    InTundra_IOHandle handle)
{
    buff->handle = handle;
    Tundra_DynArrU8_init(&buff->data);
}

void InTundra_IBuff_free(InTundra_InputBuffer *buff)
{
    buff->handle = TUNDRA_IOHANDLE_INVALID;
    Tundra_DynStkU8_free(&buff->data);
}

void InTundra_OBuff_free(InTundra_OutputBuffer *buff)
{
    buff->handle = TUNDRA_IOHANDLE_INVALID;
    Tundra_DynArrU8_free(&buff->data);
}

i64 InTundra_OBuff_write(InTundra_OutputBuffer *buff, const u8 *bytes,
    u64 num_bytes)
{
    const u64 buff_size = Tundra_DynArrU8_size(&buff->data);

    // Number of bytes to write is larger than the entire buffer, don't even
    // bother with the buffer.
    if(num_bytes > DEF_BUFF_SIZE)
    {
        // Flush whatever is in the buffer.
        i64 flush_result = InTundra_OBuff_flush(buff);

        if(flush_result < 0) return flush_result;

        i64 write_result = 
            InTundra_raw_write_bytes(buff->handle, bytes, (i64)num_bytes);

        if(write_result < 0) return write_result;

        return flush_result + write_result;
    }

    i64 bytes_written = 0;

    // The buffer would overflow from this write.
    if(DEF_BUFF_SIZE - buff_size < num_bytes)
    {
        i64 result = InTundra_OBuff_flush(buff);

        if(result < 0) return result;

        bytes_written += result;
    }

    Tundra_DynArrU8_add_mult_copy(&buff->data, bytes, num_bytes);

    return bytes_written + (i64)num_bytes;
}

i64 InTundra_IBuff_read(InTundra_InputBuffer *buff, u8 *bytes, u64 num_bytes)
{
    TUNDRA_RT_ASSERT(bytes != NULL, "Output buffer cannot be NULL.\n");

    const u64 init_buff_size = Tundra_DynStkU8_size(&buff->data);

    u64 output_idx = 0;

    // If we have enough buffered input to satisfy the request.
    if(num_bytes <= init_buff_size)
    {
        for(u64 i = 0; i < num_bytes; ++i)
        {
            bytes[output_idx++] = *Tundra_DynStkU8_top(&buff->data);
            Tundra_DynStkU8_pop(&buff->data);
        }

        return (i64)num_bytes;
    }

    // We don't have enough input in the buffer. First read whatever is in 
    // the buffer.
    
    for(u64 i = 0; i < init_buff_size; ++i)
    {
        bytes[output_idx++] = *Tundra_DynStkU8_top(&buff->data);
        Tundra_DynStkU8_pop(&buff->data);
    }

    // Increment the pointer of bytes since we've added some data to the 
    // output.
    bytes += output_idx;

    u64 remaining_bytes_needed = num_bytes - init_buff_size;

    // Don't even bother going through the buffer. 
    if(remaining_bytes_needed > DEF_BUFF_SIZE)
    {
        i64 result = InTundra_raw_read_bytes(buff->handle, bytes, 
            (i64)remaining_bytes_needed);

        if(result < 0) return result;

        // Return the raw read in bytes plus the number of bytes we already
        // read from the initially filled buffer.
        return result + (i64)init_buff_size;
    }
    u8 raw_buff[DEF_BUFF_SIZE];

    i64 result = InTundra_raw_read_bytes(buff->handle, raw_buff, 
        DEF_BUFF_SIZE);

    if(result < 0) return result;

    // We read in more bytes than we needed.
    if((u64)result > remaining_bytes_needed)
    {  
        // First get whatever bytes we need.
        Tundra_copy_mem_fwd(raw_buff, bytes, remaining_bytes_needed);

        // Add the rest to the buffer.
        const u64 num_extra_bytes = (u64)result - remaining_bytes_needed;

        u8 *extra_bytes = raw_buff + remaining_bytes_needed;

        Tundra_DynStkU8_reserve(&buff->data, num_extra_bytes);
        
        u64 extra_bytes_idx = num_extra_bytes - 1;
        
        while(true)
        {
            Tundra_DynStkU8_push_val(&buff->data, extra_bytes[extra_bytes_idx]);

            if(extra_bytes_idx == 0) break;

            --extra_bytes_idx;
        }

        return (i64)num_bytes;
    }

    // We did not read enough bytes. Simply just copy whatever we got.
    else Tundra_copy_mem_fwd(raw_buff, bytes, (u64)result);

    return result + (i64)init_buff_size;
}

i64 InTundra_OBuff_flush(InTundra_OutputBuffer *buff)
{
    const u64 buff_size = Tundra_DynArrU8_size(&buff->data);

    i64 result = InTundra_raw_write_bytes(buff->handle, 
        Tundra_DynArrU8_data(&buff->data), 
        (i64)Tundra_DynArrU8_size(&buff->data));

    if(result < 0) return result;

    TUNDRA_RT_ASSERT(result == (i64)buff_size, 
        "Failed to write entire OBuffer.\n");

    Tundra_DynArrU8_clear(&buff->data);

    return result;
}

i64 InTundra_IBuff_peek(InTundra_InputBuffer *buff)
{
    if(Tundra_DynStkU8_is_empty(&buff->data))
    {
        u8 in_buff[DEF_BUFF_SIZE];

        i64 result = 
            InTundra_raw_read_bytes(buff->handle, in_buff, DEF_BUFF_SIZE);

        if(result < 0) return result;

        Tundra_DynStkU8_push_mult_copy(&buff->data, in_buff, (u64)result);
    }

    return *Tundra_DynStkU8_top(&buff->data);
}

void InTundra_IBuff_clear(InTundra_InputBuffer *buff)
{
    Tundra_DynStkU8_clear(&buff->data);
}

void InTundra_IBuff_discard(InTundra_InputBuffer *buff, u64 num_bytes)
{
    Tundra_DynStkU8_pop_mult(&buff->data, num_bytes);
}
