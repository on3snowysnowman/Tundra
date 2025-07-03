/**
 * @file Memory.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for managing and modifying heap memory.
 * @version 0.1
 * @date 06-30-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/tundra_utils/Memory.h"

#include <string.h>
#include <stdlib.h>


void* Tundra_alloc_and_copy_memory(void* old_memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity)
{
    void* new_memory = malloc(new_byte_capacity);

    memcpy(new_memory, old_memory, num_copy_bytes);

    return new_memory;
}

uint64_t Tundra_reserve_memory(void** memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity)
{
    uint64_t remaining_space = capacity - num_used_bytes;

    uint64_t new_capacity;

    // If the capacity is already sufficient 
    if(num_reserve_bytes <= remaining_space) return capacity;

    // If the number of bytes to reserve is greater than the remaining space
    // plus another entire capacity, we can't simply double the capacity. 
    // Allocate new space that is at equal to the old elements combined
    // with the specified number of bytes to reserve.
    else if(num_reserve_bytes > capacity + remaining_space)
    {
        new_capacity = num_used_bytes + num_reserve_bytes;

        void* new_memory = Tundra_alloc_and_copy_memory(*memory, num_used_bytes, 
            new_capacity);

        free(*memory);

        *memory = new_memory;
        return new_capacity;
    }

    new_capacity = capacity * 2;

    // else -> Doubling the capacity is enough.
    void* new_memory = Tundra_alloc_and_copy_memory(*memory, num_used_bytes, 
        new_capacity);
    
    free(*memory);

    *memory = new_memory;

    return new_capacity;
}
