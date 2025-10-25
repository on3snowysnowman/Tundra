/**
 * @file MemAllocHandler.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for using the os to obtain and manage heap memory.
 * @version 0.1
 * @date 2025-10-18
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"


namespace Tundra::Internal::Mem
{

static constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 16;
static_assert(DEFAULT_ALIGNMENT >= 16, "Default alignment must be at least "
    "16.");
static_assert((DEFAULT_ALIGNMENT & (DEFAULT_ALIGNMENT - 1)) == 0, "Default "
    "alignment must be a power of 2.");

struct SystemMemData
{
    static Tundra::uint64 page_size_bytes;
};

/**
 * @brief Initializes internal components and allows memory allocation.
 *
 * @attention Must be called before any `malloc` or `free` call! 
 * 
 */
void init();

/**
 * @brief Frees the memory pointed to by `ptr`
 * 
 * @param ptr Memory to free.
 */
void free(void *ptr);

/**
 * @brief Allocates at least `num_bytes` of memory and returns a pointer to the 
 * block from internal tracked memory.
 *
 * This method is not the same as getting memory from the os, it uses our 
 * internal allocators to carve and partition cached memory.
 *
 * Block is not guaranteed to be as small as `num_bytes` due to memory 
 * alignment.
 * 
 * @param num_bytes Minimum number of bytes to allocate.
 * @return void* Pointer to the allocate memory.
 */
void* malloc(Tundra::uint64 num_bytes);

/**
 * @brief Releases memory back to the os.
 * 
 * Ensure that `num_bytes` is exactly equal to the amount requested, which 
 * should be increment of the system's memory page size.
 *
 * @param ptr Pointer to the memory to release.
 * @param num_bytes Number of bytes to release. 
 */
void release_mem_to_os(void *ptr, Tundra::uint64 num_bytes);

/**
 * @brief Prompts the os to retrieve a block of memory of `num_bytes`, returning
 * pointer to the allocated block.
 *
 * @attention Ensure that `num_bytes` is an increment of the system's memory 
 * page size, or the amount of memory allocated may be unexpected.
 * 
 * @param num_bytes Number of bytes to allocate, MUST be an increment of the 
 *    system's page size in bytes.
 *
 * @return void* Pointer to the allocated block. 
 */
void* get_mem_from_os(Tundra::uint64 num_bytes);

} // namespace Tundra::Internal::Mem
