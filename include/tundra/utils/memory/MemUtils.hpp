/**
 * @file MemUtils.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for performing operations on memory.
 * @version 0.1
 * @date 07-26-25
 *
 * @copyright Copyright (c) 2024
 *
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"


namespace Tundra
{

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
void copy_mem_fwd(const void *src, void *dst, Tundra::uint64 num_bytes);

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
void copy_mem_bwd(const void *src, void *dst, Tundra::uint64 num_bytes);

/**
 * @brief Safely copies `num_bytes` from `src` to `dst`, automatically selecting 
 *    copy direction.
 *
 * Determines whether to copy forward or backward based on the addresses of 
 * `src` and `dst` to safely handle overlapping memory regions.
 *
 * @param src Source memory block.
 * @param dst Destination memory block.
 * @param num_bytes Number of bytes to copy.
 */
void copy_mem_check_dir(const void *src, void *dst, Tundra::uint64 num_bytes);

/**
 * @brief Compares `num_bytes` from `first` and `second`, returning true if all 
 * `num_bytes` bytes are equal.
 * 
 * @param first First memory block.
 * @param second Second memory block.
 * @param num_bytes Number of bytes to compare.
 */
bool cmp_mem(const void *first, const void *second, Tundra::uint64 num_bytes);

/**
 * @brief 
 * 
 * @param mem 
 * @param index 
 * @param num_erase_bytes 
 * @param tot_bytes 
 */
void erase_shift_bytes(void *mem, Tundra::uint64 index, 
    Tundra::uint64 num_erase_bytes, Tundra::uint64 tot_bytes);




}; // namespace Tundra