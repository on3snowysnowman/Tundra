/**
 * @file SmallMemAlloc.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory sizes that are within 
 *    the defined size classes.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef TUNDRA_SMALLMEMALLOC_H
#define TUNDRA_SMALLMEMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "tundra/utils/CoreDef.h"

/**
 * The small memory allocator uses a single allocated memory arena, and carves 
 * out chunks of it in specified "buckets". These buckets are then reused when
 * no longer needed. We represent bucket sizes as "size class"s. We have a 
 * minimum size class, which means a chunk of a smaller size than that min size
 * class cannot be carved from the arena. This is for memory alignment purposes.
 * If a user requests to allocate memory larger than the maximum size class, the 
 * Large memory allocator is used, which handles allocations slightly 
 * differently. 
 * 
 */

// Most significant bit position of the minimum size class. For instance, if 
// this value is 4, then the minimum size class byte size would be 2^4 = 16
#define TUNDRA_MIN_SIZE_CLASS_MSB_POS 4

// Same as TUNDRA_MIN_SIZE_CLASS_MSB_POS but for the maximum size class.
#define TUNDRA_MAX_SIZE_CLASS_MSB_POS 12

// Number of size classes.
#define TUNDRA_NUM_SIZE_CLASSES (TUNDRA_EXPAND(TUNDRA_MAX_SIZE_CLASS_MSB_POS) -\
    TUNDRA_EXPAND(TUNDRA_MIN_SIZE_CLASS_MSB_POS) + 1)

// Byte size of the max size class.
#define TUNDRA_MAX_SIZE_CLASS_BYTE_SIZE (1ULL << \
    TUNDRA_EXPAND(TUNDRA_MAX_SIZE_CLASS_MSB_POS))

/**
 * @brief Initializes the small memory allocator. 
 * 
 * Initialization is required before allocator use.
 */
void InTundra_SmlMemAlc_init(void);

/**
 * @brief Shuts down the small memory allocator. 
 * 
 * It is good practice to shut down before the program terminates.
 */
void InTundra_SmlMemAlc_shutdown(void);

/**
 * @brief Returns true if the passed pointer is within the address range of 
 * the internal memory arena.
 * 
 * @param ptr Pointer address to check.
 * 
 * @return bool True if the pointer is in the arena.
 */
bool InTundra_SmlMemAlc_is_ptr_in_arena(void *ptr);

/**
 * @brief Frees memory allocated by the small allocator.
 * 
 * A fatal is thrown if the passed pointer was not allocated by the small 
 * allocator, or if it was and has already been freed. 
 * 
 * Note that this does not release memory to the os, it simply updates internal
 * bookkeeping and adds this memory chunk as reusable by the program.
 * 
 * Passing the byte size of the memory to free is not required. The small 
 * allocator tracks memory allocations and their size and handles it 
 * accordingly.
 *  
 * @param ptr Pointer to the memory to free.
 */
void InTundra_SmlMemAlc_free(void *ptr);

/**
 * @brief Allocates memory, carving it from the arena. Returns a pointer to the 
 * usable memory.
 * 
 * Note that this does not request a alloc call from the os, memory is carved
 * from the internal memory arena. 
 * 
 * The number of bytes to allocate is rounded to the minimum allowed size class
 * that can hold those bytes.
 * 
 * @param num_bytes Number of byte to allocate.
 * 
 * @return void* Pointer to the usable memory, with at least `num_bytes` 
 * addressable bytes. 
 */
void* InTundra_SmlMemAlc_malloc(uint64 num_bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TUNDRA_SMALLMEMALLOC_H
