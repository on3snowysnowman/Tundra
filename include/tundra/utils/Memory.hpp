/**
 * @file Memory.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for managing and modifying heap memory. 
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdint.h>
#include <math.h>


#define TUNDRA_CHECK_ALIGNMENT(ALIGNMENT)                        \
    static_assert(((ALIGNMENT) & ((ALIGNMENT) - 1)) == 0,        \
        "Alignment must be a power of 2");                       \
    static_assert((ALIGNMENT) <= 64,                             \
        "Alignment must be <= 64")


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

// Maximum byte value a memory block can be aligned to.
constexpr uint8_t DEFAULT_ALIGNMENT = 32;
constexpr uint8_t MAX_MEMORY_ALIGNMENT = 64;
constexpr uint8_t MEMORY_ALIGNMENT_32 = 32;
constexpr uint8_t MEMORY_ALIGNMENT_64 = 64;

#if defined(__x86_64__) || defined(__aarch64__) 

// Unaligned scalar copies are ok.
void scalar_copy_mem(const void *src, void *dst, 
    uint64_t num_bytes);
#else

void scalar_copy_mem(const void *src, void *dst, uint64_t num_bytes);
#endif 

// AVX2 instruction set supported. 
#ifdef __AVX2__ 

#ifndef TUNDRA_SIMD_DECLARED_32

#define TUNDRA_SIMD_DECLARED_32
void simd_copy_aligned_32_mem(const void *src, void *dst, uint64_t num_bytes);
#endif 

#ifndef TUNDRA_UNALIGNED_COPY_FUNCTION_DECLARED

#define TUNDRA_UNALIGNED_COPY_FUNCTION_DECLARED
void simd_copy_unaligned_mem(const void *src, void *dst, uint64_t num_bytes);
#endif
#endif

// SSE2 instruction set supported.
#ifdef __SSE2__ 

#ifndef TUNDRA_SIMD_DECLARED_16 

#define TUNDRA_SIMD_DECLARED_16
void simd_copy_aligned_16_mem(const void *src, void *dst, uint64_t num_bytes);
#endif // TUNDRA_SIMD_DECLARED_16 

#ifndef TUNDRA_UNALIGNED_COPY_FUNCTION_DECLARED 

#define TUNDRA_UNALIGNED_COPY_FUNCTION_DECLARED
void simd_copy_unaligned_mem(const void *src, void *dst, uint64_t num_bytes);
#endif 
#endif 

// NEON instruction set supported.
#ifdef __ARM_NEON__

#ifdef __aarch64__

#ifndef TUNDRA_SIMD_DECLARED_16
#define TUNDRA_SIMD_DECLARED_16
void simd_copy_aligned_16_mem(const void *src, void *dst, uint64_t num_bytes);
#endif

#ifndef TUNDRA_UNALIGNED_COPY_FUNCTION_DECLARED

#define TUNDRA_UNALIGNED_COPY_FUNCTION_DECLARED
void simd_copy_unaligned_mem(const void *src, void *dst, uint64_t num_bytes);
#endif

// Old NEON instruction set.
else

#ifndef TUNDRA_SIMD_DECLARED_16

#define TUNDRA_SIMD_DECLARED_16
void simd_copy_aligned_16_mem(const void *src, void *dst, uint64_t num_bytes);

#endif
#endif
#endif 

/**
 * @brief Calculates a capacity that is the smallest power of two greater than 
 * or equal to `num_bytes`.
 * 
 * @param num_bytes Minimum number of bytes to calculate from.
 *
 * @return uint64_t Calculated capacity. 
 */
inline uint64_t calculate_power_2_capacity(uint64_t num_bytes)
{
    // Calculate the number of times 2 needs to be doubled to at 
    // least reach the new capacity.
    return 1ULL << (uint8_t)ceil(log2((double)num_bytes));
}

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
    // If the capacity is already sufficient 
    if(num_reserve_bytes <= capacity - num_used_bytes) { return capacity; }

    // Calculate the number of times the capacity needs to be doubled to at 
    // least reach the new capacity.
    uint8_t num_double = (uint8_t)ceil(
        log2(
            (double)(num_used_bytes + num_reserve_bytes) / 
            (double)capacity)
        );

    // Continuously double the new capacity until it's greater than the reserved
    // bytes.
    uint64_t new_capacity = capacity * (1ULL << num_double);

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
 * @brief Copies `num_bytes` from `src` to `dst`.
 * 
 * Performs a runtime check if the memory is aligned. If it is, uses SIMD 
 * instructions for 16 or 32-byte alignment if available. Otherwise, 
 * falls back to scalar copy.
 * 
 * @param src Pointer to the source memory block.
 * @param dst Pointer to the destination memory block.
 * @param num_bytes Number of bytes to copy.
 */
void copy_mem(const void *src, void *dst, uint64_t num_bytes);

/**
 * @brief Copies `num_bytes` bytes from `src` to `dst`, assuming both are 
 * aligned to `alignment`.
 *
 * @attention Both `src` and `dst` must be aligned to the specified `alignment`. 
 * No runtime alignment checks are performed, passing unaligned pointers may 
 * result in undefined behavior.
 *
 * The `alignment` parameter must be a power of 2 and less than or equal to 64.
 *
 * Uses SIMD instructions for 16 or 32 byte alignment if available. Otherwise, 
 * falls back to scalar copy.
 *
 * @tparam alignment Alignment in bytes for both source and destination 
 *         pointers.
 * @param src Pointer to the source memory block.
 * @param dst Pointer to the destination memory block.
 * @param num_bytes Number of bytes to copy.
 */
template<uint8_t alignment>
inline void copy_aligned_mem(const void *src, void *dst, uint64_t num_bytes)
{
    TUNDRA_CHECK_ALIGNMENT(alignment);

    #ifdef TUNDRA_SIMD_DECLARED_32
    if constexpr (alignment == Tundra::Internal::MEMORY_ALIGNMENT_32)
    {
        Tundra::Internal::simd_copy_aligned_32_mem(src, dst, num_bytes);
        return;
    }
    #endif

    #ifdef TUNDRA_SIMD_DECLARED_16
    if constexpr (alignment == Tundra::Internal::MEMORY_ALIGNMENT_64)
    {
        Tundra::Internal::simd_copy_aligned_16_mem(src, dst, num_bytes);
        return;
    }
    #endif

    Tundra::Internal::scalar_copy_mem(src, dst, num_bytes);
}

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

    #ifdef _MSC_VER

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
inline void alloc_and_reserve_mem(void* *memory_output_ptr, 
    uint64_t *capacity_output_ptr, uint64_t num_bytes)
{
    uint64_t new_capacity = 
        Tundra::Internal::calculate_power_2_capacity(num_bytes);
    
    *memory_output_ptr = malloc(new_capacity);
    *capacity_output_ptr = new_capacity;
}

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
    uint64_t new_capacity = 
        Tundra::Internal::calculate_power_2_capacity(num_bytes);
    
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
