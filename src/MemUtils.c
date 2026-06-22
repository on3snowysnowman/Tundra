/**
 * @file MemUtils.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for performing operations on memory.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/utils/MemUtils.h"
#include "tundra/common/SystemInfo.h"

void Tundra_copy_mem_fwd(const void *src, void *dst, u64 num_bytes)
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
#error Not implemented.
#endif

}

void Tundra_copy_mem_bwd(const void *src, void *dst, u64 num_bytes)
{

    const u8 *src_cast = (const u8*)src;
    u8 *dst_cast = (u8*)dst;

    const u64 SHIFT_AMT = num_bytes - 1;

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
#error Not implemented.
#endif

}

void Tundra_copy_mem_safe(const void *src, void *dst, 
    u64 num_bytes)
{
    if(dst < src)
    {
        Tundra_copy_mem_fwd(src, dst, num_bytes);
        return;
    }

    Tundra_copy_mem_bwd(src, dst, num_bytes);
}

void Tundra_zero_out_mem(void *mem, u64 num_bytes)
{
    u8 *mem_cast = (u8*)(mem);

    for(u64 i = 0; i < num_bytes; ++i)
    {
        mem_cast[i] = 0;
    }
}

bool Tundra_cmp_mem(const void *first, const void *second, 
    u64 num_bytes)
{
    const u8 *first_u8_ptr = (u8 *)first;
    const u8 *second_u8_ptr = (u8 *)second;

    for(u64 i = 0; i < num_bytes; ++i)
    {
        if(*first_u8_ptr != *second_u8_ptr) { return false; }
        ++first_u8_ptr;
        ++second_u8_ptr;
    }

    return true;
}

void Tundra_erase_shift_left(void *mem, u64 index, 
    u64 num_erase_bytes, u64 tot_bytes)
{
    if(mem == NULL || tot_bytes == 0 || num_erase_bytes == 0) 
        { return; }

    u64 src_pos = index + num_erase_bytes;

    u8 *mem_cast = (u8*)(mem);

    Tundra_copy_mem_fwd(mem_cast + src_pos, mem_cast + index, 
        tot_bytes - src_pos);
}

u64 Tundra_get_cstr_len(const char *cstr)
{
    if(cstr == NULL) return 0;

    u64 length = 0;

    char c = cstr[length];

    while(c != '\0') c = cstr[++length];

    return length;
}
