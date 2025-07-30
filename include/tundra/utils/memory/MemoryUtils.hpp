/**
 * @file MemoryUtils.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for performing operations on memory.
 * @version 0.1
 * @date 07-26-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <stdint.h>


#define TUNDRA_CHECK_ALIGNMENT(ALIGNMENT)                        \
    static_assert(((ALIGNMENT) & ((ALIGNMENT) - 1)) == 0,        \
        "Alignment must be a power of 2");                       \
    static_assert((ALIGNMENT) <= 64,                             \
        "Alignment must be <= 64")


namespace Tundra
{

namespace Internal
{

// Maximum byte value a memory block can be aligned to.
constexpr uint8_t DEFAULT_ALIGNMENT = 32;
// constexpr uint8_t MAX_MEMORY_ALIGNMENT = 64;

/**
 * @brief Returns true if the address pointed to by `ptr` is aligned to 
 * `alignment`.
 * 
 * @param ptr Pointer to address.
 * @param alignment Alignment in bytes. 
 * 
 * @return [bool] True if the address is aligned.
 */
bool is_address_aligned(uintptr_t ptr, uint8_t alignment);

void scalar_copy_mem(const void *src, void *dst, 
    uint64_t num_bytes);

void scalar_copy_aligned_2_mem(const void *src, void *dst, 
    uint64_t num_bytes);

void scalar_copy_aligned_4_mem(const void *src, void *dst, 
    uint64_t num_bytes);

void scalar_copy_aligned_8_mem(const void *src, void *dst, 
    uint64_t num_bytes);

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

using AlignmentDispatchFunction = void(*)(const void*, void*, uint64_t);

#ifdef TUNDRA_SIMD_DECLARED_32
#define TUNDRA_ALIGN_DISP_ARR_LARGEST_INDEX 5
constexpr AlignmentDispatchFunction 
    alignment_dispatch_table[6]
{
    scalar_copy_mem, 
    scalar_copy_aligned_2_mem,
    scalar_copy_aligned_4_mem,
    scalar_copy_aligned_8_mem,
    simd_copy_aligned_16_mem,
    simd_copy_aligned_32_mem
};

#elif defined(TUNDRA_SIMD_DECLARED_32)
#define TUNDRA_ALIGN_DISP_ARR_LARGEST_INDEX 4
constexpr AlignmentDispatchFunction 
    alignment_dispatch_table[5]
{
    scalar_copy_mem, 
    scalar_copy_aligned_2_mem,
    scalar_copy_aligned_4_mem,
    scalar_copy_aligned_8_mem,
    simd_copy_aligned_16_mem
};

#else 
#define TUNDRA_ALIGN_DISP_ARR_LARGEST_INDEX 3
constexpr AlignmentDispatchFunction 
    alignment_dispatch_table[4]
{
    scalar_copy_mem, 
    scalar_copy_aligned_2_mem,
    scalar_copy_aligned_4_mem,
    scalar_copy_aligned_8_mem
};

#endif



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

void erase_and_shift_bytes(void *memory, uint64_t index, 
    uint64_t num_erase_bytes, uint64_t total_bytes);

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
    #ifdef __x86_64__

    asm volatile("rep movsb"
                : "=D"(dst), "=S"(src), "=c"(num_bytes)
                : "0"(dst), "1"(src), "2"(num_bytes)
                : "memory");
    #else

    TUNDRA_CHECK_ALIGNMENT(alignment);

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

    if constexpr (alignment == 8)
    {
        Tundra::Internal::scalar_copy_aligned_8_mem((const uint64_t*)src, 
            (uint64_t*)dst, num_bytes);
        return;
    }

    else if constexpr (alignment == 4)
    {
        Tundra::Internal::scalar_copy_aligned_4_mem((const uint32_t*)src,
            (uint32_t*)dst, num_bytes);
        return;
    }

    // We must be 2 byte aligned.
    else
    {
        Tundra::Internal::scalar_copy_aligned_2_mem((const uint16_t*)src,
        (uint16_t*)dst, num_bytes);
    }

    #endif
}

} // namespace Tundra
