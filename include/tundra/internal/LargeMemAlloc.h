/**
 * @file LargeMemAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory sizes that are greater 
 *    than the small allocator's defined size classes.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025 
 */

#ifndef TUNDRA_LARGEMEMALLOC_H
#define TUNDRA_LARGEMEMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "tundra/utils/CoreDef.h"

/**
 * @brief The large memory allocator functions differently than the small one. 
 * Instead of using a single allocated memory arena to carve from per each 
 * allocation request, each alloc request is its own chunk of memory obtained
 * by the os. Depending on the size of the allocation, we may cache the memory 
 * when it is freed, or release it to the os. There is a minimum byte size that 
 * can be requested from the large allocator. The main memory allocator that 
 * sits on top of the small and large allocators decides which allocator to use
 * depending on the requested byte size.
 */

/**
 * @brief Initializes the large memory allocator. 
 * 
 * Initialization is required before allocator use.
 */
void InTundra_LgMemAlc_init(void);

/**
 * @brief Shuts down the large memory allocator.
 * 
 * It is good practice to shut down before program termination.
 */
void InTundra_LgMemAlc_shutdown(void);

/**
 * @brief Frees memory allocated by the large allocator.
 * 
 * Ensure that the memory to free was allocated by the large allocator. 
 * Otherwise, undefined behavior will occur.
 * 
 * @param ptr Pointer to the memory to free.
 */
void InTundra_LgMemAlc_free(void *ptr);

/**
 * @brief Allocates memory, returning a pointer to the usable memory.
 * 
 * @param num_bytes Number of bytes to allocate.
 * 
 * @return void* Pointer to the usable memory, with at least `num_bytes` 
 * addressable bytes. 
 */
void* InTundra_LgMemAlc_malloc(uint64 num_bytes);

#ifdef __cplusplus
}   
#endif // __cplusplus

#endif // TUNDRA_LARGEMEMALLOC_H
