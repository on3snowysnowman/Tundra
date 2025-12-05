/**
 * @file MemUtils.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for performing operations on memory.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_MEMUTILS_H
#define TUNDRA_MEMUTILS_H

#include "tundra/utils/CoreDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Copies `num_bytes` from `src` to `dst` in forward (low-to-high 
 *    address) order.
 *
 * Copies memory from `src` to `dst` starting at the lowest address and moving 
 * forward. Use when `dst` is at a lower address than `src` or when regions do 
 * not overlap.
 *
 * `num_bytes` can be 0, nothing will be modified.
 *
 * @param src Source memory block.
 * @param dst Destination memory block.
 * @param num_bytes Number of bytes to copy.
 */
void Tundra_copy_mem_fwd(const void *src, void *dst, uint64 num_bytes);

/**
 * @brief Copies `num_bytes` from `src` to `dst` in backward (high-to-low 
 *    address) order.
 *
 * Copies memory from `src` to `dst` starting at the highest address and moving 
 * backward. Use when `dst` is at a higher address than `src` and regions 
 * overlap.
 *
 * @param src Source memory block.
 * @param dst Destination memory block.
 * @param num_bytes Number of bytes to copy.
 */
void Tundra_copy_mem_bwd(const void *src, void *dst, uint64 num_bytes);

/**
 * @brief Compares `num_bytes` from `first` and `second`, returning true if all 
 * `num_bytes` bytes are equal.
 * 
 * @param first First memory block.
 * @param second Second memory block.
 * @param num_bytes Number of bytes to compare.
 */
void Tundra_copy_mem_safe(const void *src, void *dst, uint64 num_bytes);

bool Tundra_cmp_mem(const void *first, const void *second, uint64 num_bytes);

void Tundra_erase_shift_left(void *mem, uint64 start_index, uint64 num_bytes, 
    uint64 shift_amt);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif
