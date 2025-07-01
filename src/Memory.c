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


void* Tundra_reallocate_memory(void* old_memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity)
{
    void* new_memory = malloc(new_byte_capacity);

    memcpy(new_memory, old_memory, num_copy_bytes);

    return new_memory;
}

void Tundra_reserve_memory(void** memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity)
{
    uint64_t remaining_space = capacity - num_used_bytes;

    // If the capacity is already sufficient 
    if(num_reserve_bytes <= remaining_space) return;

    // If the number of bytes to reserve is greater than the remaining space
    // plus another entire capacity, we can't simply double the capacity. 
    // Allocate new space that is at least enough for the old elements combined
    // with the specified number of bytes to reserve.
    else if(num_reserve_bytes > capacity + remaining_space)
    {
        void* new_memory = Tundra_reallocate_memory(*memory, num_used_bytes, 
            capacity + num_reserve_bytes);

        free(*memory);

        *memory = new_memory;
        return;
    }

    // else -> Doubling the capacity is enough.
    void* new_memory = Tundra_reallocate_memory(*memory, num_used_bytes, 
        capacity * 2);
    
    free(*memory);

    *memory = new_memory;
}
