/**
 * @file MemAlloc.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating and reserving heap memory. 
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_MEMALLOC_H
#define TUNDRA_MEMALLOC_H

#include "tundra/utils/CoreDef.h"

// Internal Methods ----------------------------------------------------------------

uint64 InTundra_calc_new_capacity_by_doubling(uint64 required_bytes,
    uint64 curr_capacity);


// Public Methods ------------------------------------------------------------------

/**
 * Allocate a block of memory at least num_bytes in size.
 *
 * Due to alignment and page‑size allocation, the actual block may be larger
 * than num_bytes. The returned block is guaranteed to be able to hold at
 * least num_bytes.
 *
 * @param num_bytes Minimum number of bytes to allocate (should be > 0).
 * @return void* Pointer to the allocated block, or nullptr on failure.
 *         Caller owns the memory and must free it..
 */
void* Tundra_alloc_mem(uint64 num_bytes);

/**
 * @brief Frees a block of memory allocated by Tundra.
 * 
 * no-op if a nullptr is passed.
 *
 * @param mem_ptr Pointer to the memory block.
 */
void Tundra_free_mem(void *mem_ptr);

/**
 * Allocate a memory block whose capacity is the smallest power of two
 * greater than or equal to num_bytes, and set the output pointers.
 *
 * @param mem_out Pointer to the variable that will be set to the new block.
 * @param capacity_out Pointer to the variable that will be set to the block's 
 *    capacity.
 * @param num_bytes Minimum number of bytes to allocate (must be > 0).
 */
void Tundra_alloc_reserve_mem(void **mem_out, uint64 *capacity_out, 
    uint64 num_bytes);

/**
 * Allocate a memory block of size num_alloc_bytes, copy num_copy_bytes from src
 * into it, and return the new block.
 *
 * @param src Source memory to copy from (may be nullptr only if 
 *    num_copy_bytes == 0).
 * @param num_alloc_bytes Number of bytes to allocate for the new block 
 *    (must be > 0).
 * @param num_copy_bytes Number of bytes to copy from src 
 *    (must be <= num_alloc_bytes).
 * @return void* Pointer to the allocated block with copied bytes, 
 *    Caller owns the memory and must free it.
 */
void* Tundra_alloc_copy_mem(const void *src, uint64 num_alloc_bytes, 
    uint64 num_copy_bytes);

/**
 * Ensure the block pointed to by *mem_out has capacity for num_used_bytes + 
 * num_reserve_bytes. If not, reallocate (doubling capacity until sufficient) 
 * and update *mem_out and *capacity_out.
 *
 * @param mem_out Pointer to the pointer to the memory block. 
 *    On reallocation it will be updated.
 * @param capacity_out Pointer to the current capacity. Will be updated on 
 *    reallocation.
 * @param num_used_bytes Number of bytes currently used in the block.
 * @param num_reserve_bytes Extra bytes required (not total).
 */
void Tundra_reserve_mem(void **mem_out, uint64* capacity_out, 
    uint64 num_used_bytes, uint64 num_reserve_bytes);

#endif