/**
 * @file Memory.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief 
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdint.h>


namespace Tundra
{

namespace Internal
{

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

} // namespace Internal


/**
 * @brief Copies `num_bytes` bytes from `src` to `dst`, assuming both are 
 * aligned to `alignment`.
 *
 * @attention Both `src` and `dst` must be aligned to the specified `alignment`. 
 * No runtime alignment checks are performed; passing unaligned pointers may 
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
void copy_aligned_mem(const void *src, void *dst, uint64_t num_bytes)
{
    static_assert((alignment & (alignment - 1)) == 0, 
        "Alignment must be power of 2");
    static_assert(alignment <= 64, "Alignment must be <= 64");

    #ifdef TUNDRA_SIMD_DECLARED_32
    if constexpr (alignment == 32)
    {
        Tundra::Internal::simd_copy_aligned_32_mem(src, dst, num_bytes);
        return;
    }
    #endif

    #ifdef TUNDRA_SIMD_DECLARED_16
    if constexpr (alignment == 16)
    {
        Tundra::Internal::simd_copy_aligned_16_mem(src, dst, num_bytes);
        return;
    }
    #endif

    Tundra::Internal::scalar_copy_mem(src, dst, num_bytes);
}

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

// /**
//  * @brief Copies `num_bytes` bytes from `source` to `dest`
//  * 
//  * @param source Pointer to the block of memory bytes are copied from.
//  * @param dest Pointer to the block of memory bytes are pasted to.
//  * @param num_bytes Number of bytes to copy.
//  */
// void copy_memory(void *source, void *dest, uint64_t num_bytes);

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
void* alloc_and_copy_memory(void *memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity);

/**
 * @brief Allocates a block of memory of size `num_bytes` aligned to `alignment` 
 * bytes.
 *
 * Provides cross-platform aligned memory allocation. The returned pointer is 
 * guaranteed to be aligned to the specified `alignment`, which must be a power 
 * of two.
 *
 * @param alignment Alignment in bytes for the allocated memory block.
 * @param num_bytes Number of bytes to allocate.
 * 
 * @return Pointer to the aligned memory block, or NULL on failure.
 */
void* aligned_alloc(uint64_t alignment, uint64_t num_bytes);

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
 * 
 * @return Capacity in bytes of the block after reservation.
 */
uint64_t reserve_memory(void **memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity);

} // namespace Tundra
