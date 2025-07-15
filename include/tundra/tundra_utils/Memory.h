/**
 * @file Memory.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for managing and modifying heap memory. 
 * @version 0.1
 * @date 06-30-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_MEMORY_H
#define TUNDRA_HGUARD_MEMORY_H

#include <stdint.h>

/**
 * @brief Allocates a new memory block with the specified capacity, copies
 * `num_copy_bytes` from the original `memory` block, and returns a pointer to
 * the new block.
 *
 * @param memory Pointer to the original memory block.
 * @param num_copy_bytes Number of bytes to copy from the original block.
 * @param new_byte_capacity Size in bytes of the new memory block to allocate.
 * @return Pointer to the newly allocated memory block, or NULL on failure.
 */
void* Tundra_alloc_and_copy_memory(void *memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity);

/**
 * @brief Ensures a memory block has at least the requested capacity, 
 * reallocating if necessary. Returns the capacity of the new block. 
 *
 * If the current available capacity is less than `num_reserve_bytes`, 
 * reallocates the memory block to a larger capacity, copies the used bytes, 
 * frees the old block, and updates the pointer.
 * 
 * If the memory needs to be reallocated, the default behavior is to expand
 * the memory block to twice its size. If this isn't enough to hold the extra
 * `num_reserve_bytes`, the new block size will be the exact amount needed
 * to store the extra bytes.
 *
 * @param memory Pointer to the pointer to the memory block.
 * @param num_reserve_bytes Number of extra bytes to reserve in front of 
 *                          `num_used_bytes`.
 * @param num_used_bytes Number of bytes currently used in the memory block.
 * @param capacity Current capacity of the memory block in bytes.
 * @return Capacity of the block after reservation.
 */
uint64_t Tundra_reserve_memory(void **memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity);


#endif // TUNDRA_HGUARD_MEMORY_H