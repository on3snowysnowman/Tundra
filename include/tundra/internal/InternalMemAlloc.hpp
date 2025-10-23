/**
 * @file InternalMemoryAlloc.hpp
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
 * block.
 *
 * Block is not guaranteed to be as small as `num_bytes` due to memory 
 * alignment.
 * 
 * @param num_bytes Minimum number of bytes to allocate.
 * @return void* Pointer to the allocate memory.
 */
void* malloc(Tundra::uint64 num_bytes);

}