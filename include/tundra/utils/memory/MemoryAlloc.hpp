/**
 * @file Memory.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating and reserving heap memory. 
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdint.h>
#include <math.h>

#include "tundra/utils/memory/MemoryUtils.hpp"

#if !defined(__aarch64__) && !defined(__x86_64)
#error Unsupported Architecture
#endif

#if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
#error Unsupported OS
#endif

#if defined(__linux__) || defined(__APPLE__)

#include <stdlib.h>

#else // Windows

#include <malloc.h>

#endif

namespace Tundra
{ 

/**
 * @brief Frees aligned memory.
 * 
 * @param mem Aligned memory to free.
 */
void aligned_free(void *mem);

/**
 * @brief Allocates a new memory block with the specified capacity, copies
 * `num_copy_bytes` from the original block `memory`, and returns a pointer to
 * the new block.
 *
 * @param memory Pointer to the original memory block.
 * @param num_copy_bytes Number of bytes to copy.
 * @param new_byte_capacity Size in bytes to allocate for the new block.
 * 
 * @return Pointer to the newly allocated memory, or NULL on failure.
 */
void* alloc_and_copy_mem(const void *memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity);

// Forward declaration. 
template<uint8_t alignment>
void* alloc_and_copy_aligned_mem(const void *memory, uint64_t num_copy_bytes,
    uint64_t new_byte_capacity);
    

namespace Internal
{

template<typename T>
struct always_false { static constexpr bool value = false; };

/**
 * @brief Given a set of bits (64 bits), returns the number of leading zeros
 * behind the most significant set bit.
 * 
 * @param bits Bits to check.
 *
 * @return uint32_t Number of leading zeros. 
 */
uint32_t get_num_leading_zeros(uint64_t bits);

/**
 * @brief Given a set of bits (64 bits), returns the number of trailing zeros
 * in front of the most significant set bit.
 * 
 * @param bits Bits to check.
 *
 * @return uint32_t Number of trailing zeros. 
 */
uint32_t get_num_trailing_zeros(uint64_t bits);

/**
 * @brief Calculates the minimum capacity that can store `required_bytes` by 
 * doubling `capacity` until that point is reached. 
 * 
 * This is done efficiently through bit operations, not a brute force while 
 * loop and branch check.
 *
 * @param required_bytes Minimum number of bytes the capacity needs to hold.
 * @param capacity Current capacity that will be recalculated.
 *
 * @return uint64_t New capacity calculated from doubling the old `capacity`. 
 */
uint64_t calc_new_capacity_by_doubling(uint64_t required_bytes, 
    uint64_t capacity);

/**
 * @brief Underlying reserve method. Ensures a memory block has at least the 
 * requested capacity, reallocating if necessary. Returns the capacity of the 
 * block. 
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
 * If `alignment` is not 0, the memory is validly aligned to that value.
 * Since this is an internal method, we can guarantee that any non 0 value 
 * is a valid alignment.
 * @tparam alignment Alignment of the memory block, or 0 if it is not aligned.
 *
 * @param memory Pointer to the pointer to the memory block.
 * @param num_reserve_bytes Number of extra bytes to reserve in front of 
 *                          `num_used_bytes`.
 * @param num_used_bytes Number of bytes currently used in the memory block.
 * @param capacity Current capacity of the memory block in bytes.
 * 
 * @return Capacity in bytes of the block after reservation.
 */
template<uint8_t alignment>
inline uint64_t underlying_reserve_mem(void **memory, uint64_t num_reserve_bytes,
    uint64_t num_used_bytes, uint64_t capacity)
{
    uint64_t total_required_bytes = num_used_bytes + num_reserve_bytes;

    // If the capacity is already sufficient 
    if(total_required_bytes <= capacity) { return capacity; }

    // Calculate the new capacity.
    uint64_t new_capacity = Tundra::Internal::calc_new_capacity_by_doubling(
        total_required_bytes, capacity);

    void *new_memory = NULL;

    // Since this is an internal method, we can ensure that `alignment` is valid
    // for non 0 values, so if it is 0, this is a flag that we are not aligned.
    if constexpr (alignment == 0)
    {
        new_memory = Tundra::alloc_and_copy_mem(*memory, num_used_bytes, 
            new_capacity);
        free(*memory);
    }

    // We are aligned.
    else 
    {
        new_memory = Tundra::alloc_and_copy_aligned_mem<alignment>(
            *memory, num_used_bytes, new_capacity);
        Tundra::aligned_free(*memory);
    }

    *memory = new_memory;

    return new_capacity;
}

} // namespace Internal

/**
 * @brief Allocates a block of memory of size `num_bytes` aligned to `alignment` 
 * bytes.
 * 
 * @attention To free this memory, `aligned_free()` must be called with it. 
 *
 * Provides cross-platform aligned memory allocation. The returned pointer is 
 * guaranteed to be aligned to the specified `alignment`, which must be a power 
 * of two.
 *
 * @tparam alignment 
 *
 * @param num_bytes Number of bytes to allocate.
 * 
 * @return Pointer to the aligned memory block, or NULL on failure.
 */
template<uint8_t alignment>
inline void* aligned_alloc(uint64_t num_bytes)
{
    TUNDRA_CHECK_ALIGNMENT(alignment);

    #ifdef _WIN32

    return _aligned_malloc(num_bytes, alignment);
    #else

    void* aligned_mem = NULL;
    posix_memalign(&aligned_mem, alignment, num_bytes);
    return aligned_mem;
    #endif
}

/**
 * @brief Allocates a memory block with a capacity that is the smallest power 
 * of two greater than or equal to `num_bytes`, and sets the output pointers.
 *
 * `num_bytes` must be non zero.
 *
 * Calculates the minimum capacity needed to store `num_bytes` by doubling until 
 * sufficient, allocates the memory, and updates `memory_output` and 
 * `capacity_output`.
 *
 * @param memory_output_ptr Pointer to a void* variable that will be set to the 
 *    new memory block.
 * @param capacity_output_ptr Pointer to a uint64_t variable that will be set to 
 *    the new block's capacity.
 * @param num_bytes Minimum number of bytes to allocate.
 *
 * @return void* Pointer to the newly allocated memory block.
 */
void alloc_and_reserve_mem(void* *memory_output_ptr, 
    uint64_t *capacity_output_ptr, uint64_t num_bytes);

/**
 * @brief Allocates an aligned memory block with a capacity that is the smallest 
 * power of two greater than or equal to `num_bytes`, and sets the output 
 * pointers.
 *
 * THe memory will be aligned to `alignment`.
 *
 * Calculates the minimum capacity needed to store `num_bytes` by doubling until 
 * sufficient, allocates the memory, and updates `memory_output` and `capacity_output`.
 *
 * @tparam alignment Alignment in bytes of the memory 
 *
 * @param memory_output_ptr Pointer to a void* variable that will be set to the 
 *    new memory block.
 * @param capacity_output_ptr Pointer to a uint64_t variable that will be set to 
 *    the new block's capacity.
 * @param num_bytes Minimum number of bytes to allocate.
 *
 * @return void* Pointer to the newly allocated memory block.
 */
template<uint8_t alignment>
inline void alloc_and_reserve_aligned_mem(void* *memory_output_ptr,
    uint64_t *capacity_output_ptr, uint64_t num_bytes)
{
    uint64_t new_capacity;

    // Num bytes is already a power of 2.
    if((num_bytes & (num_bytes - 1)) == 0) 
    {
        new_capacity = num_bytes;
    }

    else
    {
        new_capacity = 
           Tundra::Internal::calc_new_capacity_by_doubling(num_bytes, 2);
    }
    
    *memory_output_ptr = aligned_alloc<alignment>(new_capacity);
    *capacity_output_ptr = new_capacity;
}

/**
 * @brief Allocates a new memory block aligned to `alignment` with the specified
 * capacity, copies `num_copy_bytes` from the original aligned block `memory`, 
 * and returns a pointer to the new block.
 * 
 * @note `memory` must be aligned  to `alignment`. The new block will also be 
 * aligned to `alignment`.
 * 
 * @tparam alignment Alignment in bytes for both `memory` and the new memory.
 * 
 * @param memory Pointer to the original aligned memory block.
 * @param num_copy_bytes Number of bytes to copy.
 * @param new_byte_capacity Size in bytes to allocate for the new block.
 * 
 * @return void* Pointer to the newly allocated aligned memory, or NULL on 
 *    failure.
 */
template<uint8_t alignment>
inline void* alloc_and_copy_aligned_mem(const void *memory, uint64_t num_copy_bytes,
    uint64_t new_byte_capacity)
{
    void *new_memory = Tundra::aligned_alloc<alignment>(new_byte_capacity);

    Tundra::copy_aligned_mem<alignment>(memory, new_memory, num_copy_bytes);

    return new_memory;
}

/**
 * @brief Ensures a memory block has at least the requested capacity, 
 * reallocating if necessary. Returns the capacity of the block. 
 *
 * If the current capacity is less than the needed capacity, the block's new 
 * capacity will be doubled until the required capacity is at least reached. 
 * This means that the resizing will follow the standard behavior of doubling 
 * the capacity instead of simply reallocating exactly the bytes needed to 
 * reach the new capacity.
 * 
 * @param memory Pointer to the pointer to the memory block.
 * @param num_reserve_bytes Number of extra bytes to reserve in front of 
 *                          `num_used_bytes`.
 * @param num_used_bytes Number of bytes currently used in the memory block.
 * @param capacity Current capacity of the memory block in bytes.
 * 
 * @return Capacity in bytes of the block after reservation.
 */
uint64_t reserve_mem(void **memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity);

/**
 * @brief Ensures a memory block has at least the requested capacity, 
 * reallocating if necessary. Returns the capacity of the block. 
 *
 * @attention `memory` must be aligned to `alignment`. If this method 
 * needs to reallocate the memory, the new block will also be aligned.
 *
 * If the current capacity is less than the needed capacity, the block's new 
 * capacity will be doubled until the required capacity is at least reached. 
 * This means that the resizing will follow the standard behavior of doubling 
 * the capacity instead of simply reallocating exactly the bytes needed to 
 * reach the new capacity.
 * 
 *@tparam alignment Alignment in bytes of the memory.
 *
 * @param memory Pointer to the pointer to the memory block.
 * @param num_reserve_bytes Number of extra bytes to reserve in front of 
 *                          `num_used_bytes`.
 * @param num_used_bytes Number of bytes currently used in the memory block.
 * @param capacity Current capacity of the memory block in bytes.
 * 
 * @return Capacity in bytes of the block after reservation.
 */
template<uint8_t alignment>
uint64_t reserve_aligned_mem(void **memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity)
{
    TUNDRA_CHECK_ALIGNMENT(alignment);

    return Tundra::Internal::underlying_reserve_mem<alignment>(memory, 
        num_reserve_bytes, num_used_bytes, capacity);
}

} // namespace Tundra
