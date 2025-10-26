/**
 * @file MemAlloc.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating and reserving heap memory. 
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2024
 *
*/

#include "tundra/utils/memory/MemAlloc.hpp"
#include "tundra/utils/BitUtils.hpp"
#include "tundra/internal/MemAllocHandler.hpp"
#include "tundra/utils/memory/MemUtils.hpp"

// Internal --------------------------------------------------------------------

Tundra::uint64 Tundra::Internal::calc_new_capacity_by_doubling(
    Tundra::uint64 required_bytes, Tundra::uint64 curr_capacity)
{
    // Calculate how many full capacity units are needed to fit the required 
    // bytes. This is a ceiling division: round up (required_bytes / capacity).
    Tundra::uint64 overfill_ratio = 
        (required_bytes + curr_capacity - 1) / curr_capacity;
    
    // Find the position of the most significant set bit in the overfill ratio.
    // This is equivalent to floor(log2(overfill_ratio)), which tells us how
    // many doublings are needed to just reach the ratio.
    Tundra::uint8 msb_position = 63 - get_num_lead_zeros(overfill_ratio);

    // If the ratio is not already a power of 2, we need to round up,
    // so we increment the position to get ceil(log2(overfill_ratio)).
    msb_position = ((overfill_ratio & (overfill_ratio - 1)) == 0) ? 
        msb_position : msb_position + 1;
        
    // Double the capacity the required number of times to ensure it's large
    // enough to hold the requested number of bytes.
    return curr_capacity << msb_position;
}

// Public ----------------------------------------------------------------------

void* Tundra::alloc_mem(Tundra::uint64 num_bytes)
{
    return Tundra::Internal::Mem::malloc(num_bytes);
}

void Tundra::free_mem(void *mem_ptr)
{
    Tundra::Internal::Mem::free(mem_ptr);
}

void Tundra::alloc_reserve_mem(void **mem_out, Tundra::uint64 *capacity_out,
    Tundra::uint64 num_bytes)
{
    // Default case, num_bytes is a power of 2. Check on next line.
    Tundra::uint64 new_capacity = num_bytes;

    // Num bytes is not a power of 2.
    if((num_bytes & (num_bytes - 1)) != 0)
    {
        new_capacity = 
            Tundra::Internal::calc_new_capacity_by_doubling(num_bytes, 2);
    }

    *mem_out = alloc_mem(new_capacity);
    *capacity_out = new_capacity;
}

void* Tundra::alloc_copy_mem(const void *src, Tundra::uint64 num_alloc_bytes, 
    Tundra::uint64 num_copy_bytes)
{
    void *new_mem = alloc_mem(num_alloc_bytes);

    if(new_mem == nullptr) { return nullptr; }

    Tundra::copy_mem_fwd(src, new_mem, num_copy_bytes);

    return new_mem;
}

void Tundra::reserve_mem(void **mem_out, Tundra::uint64* capacity_out, 
    Tundra::uint64 num_used_bytes, Tundra::uint64 num_reserve_bytes)
{
    Tundra::uint64 tot_req_bytes = num_used_bytes + num_reserve_bytes;

    // If the capacity is already sufficient.
    if(tot_req_bytes <= *capacity_out) { return; }

    Tundra::uint64 new_cap = 
        Tundra::Internal::calc_new_capacity_by_doubling(tot_req_bytes, 
        *capacity_out);

    // Create a new block with our calculated sufficient capacity, then copy 
    // over the old bytes from the original block passed in.
    void *new_mem = Tundra::alloc_copy_mem(*mem_out, new_cap, num_used_bytes);
    
    free_mem(*mem_out);

    *mem_out = new_mem;
}

void Tundra::init_mem_sys()
{
    Tundra::Internal::Mem::init();
}
