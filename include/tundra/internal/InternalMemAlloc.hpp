/**
 * @file InternalMemoryAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Internal methods for using the os to obtain and manage heap memory.
 * @version 0.1
 * @date 2025-10-18
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once

#include "tundra/internal/SystemCheck.hpp"
#include "tundra/utils/CoreTypes.hpp"


namespace Tundra::Internal::Mem
{

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
