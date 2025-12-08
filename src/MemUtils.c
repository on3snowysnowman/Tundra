/**
 * @file MemUtils.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for performing operations on memory.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/utils/MemUtils.h"
#include "tundra/utils/SystemInfo.h"

void Tundra_copy_mem_fwd(const void *src, void *dst, uint64 num_bytes)
{

#ifdef TUNDRA_SYS_x86_64

    asm volatile
    (
        "rep movsb" // Copy bytes forward.
        :
        : "D"(dst), "S"(src), "c"(num_bytes)
        : "memory", "cc"
    );

#else // ARM
#error Implement this.
#endif

}

void Tundra_copy_mem_bwd(const void *src, void *dst, uint64 num_bytes)
{

    const uint8 *src_cast = (const uint8*)src;
    uint8 *dst_cast = (uint8*)dst;

    const uint8 SHIFT_AMT = num_bytes - 1;

    // Place src at the end of the copy section.
    src_cast += SHIFT_AMT;
    
    // Do the same for dst.
    dst_cast += SHIFT_AMT;

#ifdef TUNDRA_SYS_x86_64

    asm volatile 
    (
        "std\n\t"          // Set direction flag
        "rep movsb\n\t"    // Copy bytes backward
        "cld\n\t"          // Clear direction flag.
        :     
        : "D"(dst_cast), "S"(src_cast), "c"(num_bytes)           
        : "memory", "cc"
    );

#else // ARM
#error Implement this.
#endif

}

void Tundra_copy_mem_check_dir(const void *src, void *dst, 
    uint64 num_bytes)
{
    if(dst < src)
    {
        Tundra_copy_mem_fwd(src, dst, num_bytes);
        return;
    }

    Tundra_copy_mem_bwd(src, dst, num_bytes);
}

bool Tundra_cmp_mem(const void *first, const void *second, 
    uint64 num_bytes)
{
    return __builtin_memcmp(first, second, num_bytes) == 0;
}

void Tundra_erase_shift_left(void *mem, uint64 index, 
    uint64 num_erase_bytes, uint64 tot_bytes)
{
    if(mem == NULL || tot_bytes == 0 || num_erase_bytes == 0) 
        { return; }

    uint64 src_pos = index + num_erase_bytes;

    uint8 *mem_cast = (uint8*)(mem);

    Tundra_copy_mem_fwd(mem_cast + src_pos, mem_cast + index, 
        tot_bytes - src_pos);
}
