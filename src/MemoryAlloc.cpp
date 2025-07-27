/**
 * @file Memory.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating and reserving heap memory. 
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/utils/memory/MemoryAlloc.hpp"

// Internal --------------------------------------------------------------------

uint32_t Tundra::Internal::get_num_leading_zeros(uint64_t bits)
{
    uint64_t num_trailing;
 
    #ifdef __x86_64__
    
    __asm__ __volatile__
    (
        "bsr %1, %0\n\t"         // Find index of MSB.
        "xor $63, %0\n\t"        // Convert to count of leading zeros.
        "cmovz %1, %0"           // If input was zero, result = 64.
        : "=&r"(num_trailing)
        : "r"(bits)
        : "cc"
    );
    
    #else

    __asm__ __volatile__
    (
        "clz %0, %1"
        : "=r"(result)
        : "r"(x)
    );

    #endif

    return (uint32_t)num_trailing;
}

uint32_t Tundra::Internal::get_num_trailing_zeros(uint64_t bits)
{
    uint64_t num_trailing;

    #ifdef __x86_64__

    __asm__ __volatile__
    (
        "bsf %1, %0\n\t"         // Bit Scan Forward → counts trailing zeros
        "cmovz %1, %0"           // If input was zero, return 64
        : "=&r"(num_trailing)
        : "r"(bits)
        : "cc"
    );

    return (uint32_t)num_trailing;

    #else
    #error "Trailing zero count not implemented for this platform."
    #endif
}

uint64_t Tundra::Internal::calc_new_capacity_by_doubling(
    uint64_t required_bytes, uint64_t capacity)
{
    // Calculate how many full capacity units are needed to fit the required 
    // bytes. This is a ceiling division: round up (required_bytes / capacity).
    uint64_t overfill_ratio = 
        (required_bytes + capacity - 1) / capacity;
    
    // Find the position of the most significant set bit in the overfill ratio.
    // This is equivalent to floor(log2(overfill_ratio)), which tells us how
    // many doublings are needed to just reach the ratio.
    uint8_t msb_position = 63 - get_num_leading_zeros(overfill_ratio); // NOLINT

    // If the ratio is not already a power of 2, we need to round up,
    // so we increment the position to get ceil(log2(overfill_ratio)).
    msb_position = ((overfill_ratio & (overfill_ratio - 1)) == 0) ? 
        msb_position : msb_position + 1;
        
    // Double the capacity the required number of times to ensure it's large
    // enough to hold the requested number of bytes.
    return capacity << msb_position;
}


// Public ----------------------------------------------------------------------


void Tundra::alloc_and_reserve_mem(void* *memory_output_ptr, 
    uint64_t *capacity_output_ptr, uint64_t num_bytes)
{
    uint64_t new_capacity;

    // Num bytes is already a power of 2.
    if((num_bytes & (num_bytes - 1)) == 0) 
    {
        new_capacity = num_bytes;
    }

    else
    {
        new_capacity = 
           Tundra::Internal::calc_new_capacity_by_doubling(num_bytes, 2);
    }

    
    *memory_output_ptr = malloc(new_capacity);
    *capacity_output_ptr = new_capacity;
}

void Tundra::aligned_free(void *mem)
{
    #ifdef _WIN32
    _aligned_free(mem);
    #else
    free(mem);
    #endif
}

void* Tundra::alloc_and_copy_mem(const void *old_memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity)
{
    void *new_memory = malloc(new_byte_capacity);

    if(!(bool)new_memory) return NULL;

    Tundra::copy_mem(old_memory, new_memory, num_copy_bytes);

    return new_memory;
}

uint64_t Tundra::reserve_mem(void **memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity)
{
    return Tundra::Internal::underlying_reserve_mem<0>(memory, 
        num_reserve_bytes, num_used_bytes, capacity);
}
