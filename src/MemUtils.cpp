/**
 * @file MemUtils.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for performing operations on memory.
 * @version 0.1
 * @date 07-26-25
 *
 * @copyright Copyright (c) 2024
 *
*/

#include "tundra/utils/memory/MemUtils.hpp"
#include "tundra/utils/SystemInfo.hpp"


void Tundra::copy_mem_fwd(const void *src, void *dst, Tundra::uint64 num_bytes)
{

#ifdef TUNDRA_SYS_x86_64

    asm volatile
    (
        "rep movsb" // Copy bytes forward.
        : "=D"(dst), "=S"(src), "=c"(num_bytes)
        : "0"(dst), "1"(src), "2"(num_bytes)
        : "memory"
    );

#else // ARM
#error Implement this.
#endif

}

void Tundra::copy_mem_bwd(const void *src, void *dst, Tundra::uint64 num_bytes)
{

#ifdef TUNDRA_SYS_x86_64

    asm volatile 
    (
        "std\n\t"          // Set direction flag
        "rep movsb\n\t"    // Copy bytes backward
        "cld\n\t"         
        : "+D"(dst), "+S"(src), "+c"(num_bytes) 
        :                 
        : "memory"        
    );

#else // ARM
#error Implement this.
#endif

}

void Tundra::copy_mem_check_dir(const void *src, void *dst, 
    Tundra::uint64 num_bytes)
{
    if(dst < src)
    {
        Tundra::copy_mem_fwd(src, dst, num_bytes);
        return;
    }

    Tundra::copy_mem_bwd(src, dst, num_bytes);
}

bool Tundra::cmp_mem(const void *first, const void *second, 
    Tundra::uint64 num_bytes)
{
    return __builtin_memcmp(first, second, num_bytes) == 0;
}
/**
 * @brief Remove a contiguous range of bytes by shifting trailing bytes down.
 *
 * Moves the bytes in the range [index + num_erase_bytes, tot_bytes) to start
 * at byte offset index, preserving their relative order. This effectively
 * "erases" num_erase_bytes bytes at the given byte offset and compacts the
 * remainder of the buffer downwards.
 *
 * The user is responsible for updating any external size bookkeeping.
 *
 * @param mem Memory block to modify.
 * @param index Byte offset into mem where erasure begins.
 * @param num_erase_bytes Number of bytes to remove.
 * @param tot_bytes Total number of bytes in `mem`.
 */
void Tundra::erase_shift_bytes(void *mem, Tundra::uint64 index, 
    Tundra::uint64 num_erase_bytes, Tundra::uint64 tot_bytes)
{
    if(mem == nullptr || tot_bytes == 0 || num_erase_bytes == 0) 
        { return; }

    Tundra::uint64 src_pos = index + num_erase_bytes;

    Tundra::uint8 *mem_cast = reinterpret_cast<Tundra::uint8*>(mem);

    Tundra::copy_mem_fwd(mem_cast + src_pos, mem_cast + index, 
        tot_bytes - src_pos);
}