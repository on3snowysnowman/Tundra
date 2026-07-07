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
#include "tundra/common/Core.h"
#include "tundra/common/ErrorDef.h"


static u64 get_buff_size(InTundra_IOBuffer *buff)
{
    return Tundra_DynArrU8_size(&buff->data);
}

#define DEF_BUFF_SIZE 32

// DEF_BUFF_SIZE should be a power of 2.
// TUNDRA_CT_ASSERT((DEF_BUFF_SIZE & (DEF_BUFF_SIZE - 1)) == 0);
TUNDRA_CT_ASSERT(IS_POW_TWO(DEF_BUFF_SIZE));

void InTundra_IOBuff_init(InTundra_IOBuffer *buff, InTundra_IOHandle handle)
{
    buff->cursor_pos = 0;
    buff->handle = handle;
    Tundra_DynArrU8_init_cap(&buff->data, DEF_BUFF_SIZE);
}

void InTundra_IOBuff_free(InTundra_IOBuffer *buff)
{
    Tundra_DynArrU8_free(&buff->data);
}

/**
 * @brief Checks the result of a write to a handle, erroring if the result is
 * negative or if the amount does not match the expected number of bytes.
 * 
 * @param result Result of the write.
 * @param expected_bytes Number of bytes that was expected to be written.
 */
static void check_write_err(i64 result, u64 expected_bytes)
{
    // If error
    if(result < 0)
    {
        TUNDRA_FATAL("Failed to write IOBuffer.", 0);
        return;
    }

    // For now, error if we didn't write all bytes in the buffer.

    TUNDRA_RT_ASSERT((i64)expected_bytes == result, 
        "Failed to write entire IOBuffer.\n", 0);
}

i64 InTundra_IOBuff_write_bytes(InTundra_IOBuffer *buff, const u8 *bytes,
    u64 num_bytes)
{
    const u64 buff_size = get_buff_size(buff);

    // If the size of the write is larger than the entire buffer.
    if(num_bytes > DEF_BUFF_SIZE)
    {
        // Simply just flush whatever is in the buffer and write the incoming
        // bytes directly.

        InTundra_IOBuff_flush(buff);
        i64 result = InTundra_raw_write_bytes(buff->handle, bytes, (i64)num_bytes);
        check_write_err(result, num_bytes);
        return result;
    }

    // If the size of the write is NOT larger than the entire buffer, but the 
    // buffer would overflow if we added all the incoming bytes to it.
    if(buff_size + num_bytes > DEF_BUFF_SIZE)
    {
        const u64 bytes_free_in_buff = DEF_BUFF_SIZE - buff_size;

        // Write whatever we can to the buffer to fill up the buffer's free 
        // space.
        Tundra_DynArrU8_add_mult_copy(&buff->data, bytes, bytes_free_in_buff);

        // Write the full buffer to the handle.
        i64 result = InTundra_IOBuff_flush(buff);

        if(result < 0) return result;

        const u64 bytes_left_to_write = num_bytes - bytes_free_in_buff;

        // Write the leftover bytes that would've overflown the buffer into the 
        // now empty flushed buffer.
        Tundra_DynArrU8_add_mult_copy(&buff->data, bytes + bytes_free_in_buff,
            bytes_left_to_write);

        // Bytes written is the sum of the flush write result and the bytes 
        // written to the emptied buffer.
        return result +(i64)bytes_left_to_write;
    }

    // Buffer won't overfill, simply just add the bytes.
    Tundra_DynArrU8_add_mult_copy(&buff->data, bytes, num_bytes);

    return (i64)num_bytes;
}

static i64 fill_buffer(InTundra_IOBuffer *buff)
{
    u8 in_buffer[DEF_BUFF_SIZE];

    i64 result = InTundra_raw_read_bytes(TUNDRA_IOHANDLE_STDIN, in_buffer, 
        DEF_BUFF_SIZE);

    if(result >= 0) 
        Tundra_DynArrU8_add_mult_copy(&buff->data, in_buffer, result);

    return result;
}

i64 InTundra_IOBuff_read_bytes(InTundra_IOBuffer *buff, u8 *bytes, 
    u64 num_bytes)
{
    // fprintf(stderr, "Reading in: %llu bytes\n", num_bytes);

    const u64 initial_buff_size = get_buff_size(buff);

    // fprintf(stderr, "Buff size: %llu\n", initial_buff_size);

    // Simple case. We have enough already in the buffer to feed back.
    if(num_bytes <= initial_buff_size)
    {
        const u8 *buff_data = Tundra_DynArrU8_data(&buff->data);

        Tundra_copy_mem_fwd(buff_data, bytes, num_bytes);

        Tundra_DynArrU8_erase_multiple(&buff->data, 0, num_bytes);

        return (i64)num_bytes;
    }

    // More bytes are requested than are in the buffer. First, read in 
    // whatever is in the buffer to the output.

    Tundra_copy_mem_fwd(Tundra_DynArrU8_data(&buff->data), bytes, 
        initial_buff_size);
    Tundra_DynArrU8_clear(&buff->data);

    // Buffer is now empty, we still need to read in more bytes.

    // Iterate the bytes pointer since we've added in some (but not enough)
    // bytes.
    bytes += initial_buff_size; 

    const u64 remaining_needed = num_bytes - initial_buff_size;

    // The remaining needed bytes are larger than the entire buffer, so no need
    // to even use the buffer. Read directly into the caller output.
    if(remaining_needed > DEF_BUFF_SIZE)
    {
        // Add the init buff size onto the output buffer since we've already
        // read in the previous buffer into the output.
        return InTundra_raw_read_bytes(TUNDRA_IOHANDLE_STDIN, 
            bytes, (i64)remaining_needed);
    }

    // The remaining needed would fit inside the buffer fill the buffer with 
    // as many bytes as we can.

    // Temporary buffer to read into.
    u8 readin_buff[DEF_BUFF_SIZE];

    i64 result = InTundra_raw_read_bytes(TUNDRA_IOHANDLE_STDIN, readin_buff,
        DEF_BUFF_SIZE);

    if(result < 0)
    {
        // If we failed to read in any more bytes, return the bytes we know
        // we've fed to the output which was whatever was initially in the 
        // buffer.
        return (i64)initial_buff_size;
    }

    // We didn't read in enough for what we needed. Just copy to the output 
    // whatever we did read,
    if(result <= (i64)remaining_needed)
    {

        Tundra_copy_mem_fwd(readin_buff, bytes, (u64)result);
        return result;
    }

    // We read in more bytes than what we needed. Copy what we need to the
    // output then feed the rest into the IO buffer.

    Tundra_copy_mem_fwd(readin_buff, bytes, remaining_needed);

    Tundra_DynArrU8_add_mult_copy(&buff->data, readin_buff + remaining_needed,
        (u64)result - remaining_needed);

    return (i64)num_bytes;
}

u64 InTundra_IOBuff_size(InTundra_IOBuffer *buff)
{
    return Tundra_DynArrU8_size(&buff->data);
}

char InTundra_IOBuff_peek(InTundra_IOBuffer *buff)
{
    if(InTundra_IOBuff_size(buff) == 0)
    {
        // Fill buffer 
        i64 result = InTundra_raw_read_bytes(TUNDRA_IOHANDLE_STDIN, buff,
            DEF_BUFF_SIZE);

        TUNDRA_RT_ASSERT(result >= 0, "Failed to fill IOBuffer with error: "
            "%s.\n", Tundra_err_to_rdbl(result));
    }

    return (char)*Tundra_DynArrU8_front(&buff->data);
}

i64 InTundra_IOBuff_flush(InTundra_IOBuffer *buff)
{        
    const u64 buff_size = get_buff_size(buff);

    if(buff_size == 0) return 0;

    i64 result = InTundra_raw_write_bytes(buff->handle, 
        Tundra_DynArrU8_data(&buff->data), 
        (i64)buff_size);

    check_write_err(result, buff_size);
    
    Tundra_DynArrU8_clear(&buff->data);

    return result;
}
