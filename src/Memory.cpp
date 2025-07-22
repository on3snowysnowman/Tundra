/**
 * @file Memory.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for managing and modifying heap memory. 
 * @version 0.1
 * @date 07-16-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/utils/Memory.hpp"

#ifdef _MSC_VER
#include <malloc.h>
#else
#include <stdlib.h>
#endif




// Internal --------------------------------------------------------------------

#if defined(__i386__) || defined(__x86_64__) || defined(__aarch64__) 

// Unaligned scalar copies are ok.
void Tundra::Internal::scalar_copy_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    uint8_t *src_iter = (uint8_t*)src;
    uint8_t *dst_iter = (uint8_t*)dst;

    while(num_bytes >= 8)
    {
        *((uint64_t*)dst_iter) = *((uint64_t*)src_iter);
        src_iter += 8;
        dst_iter += 8;
        num_bytes -= 8;
    }

    while(num_bytes >= 4)
    {
        *((uint32_t*)dst_iter) = *((uint32_t*)src_iter);
        src_iter += 4;
        dst_iter += 4;
        num_bytes -= 4;
    }

    while(num_bytes >=2)
    {
        *((uint16_t*)dst_iter) = *((uint16_t*)src_iter);
        src_iter += 2;
        dst_iter += 2;
        num_bytes -= 2;
    }

    while(num_bytes > 0)
    {
        *dst_iter = *src_iter;
        ++src_iter;
        ++dst_iter;
        --num_bytes;
    }
}
#else

void Tundra::Internal::scalar_copy_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    // TODO: Improve this.
    for(uint64_t i = 0; i < num_bytes; ++i)
    {
        ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
    }
}
#endif 

// AVX2 instruction set supported. 
#ifdef __AVX2__ 

#ifndef TUNDRA_SIMD_DEFINED_32

#define TUNDRA_SIMD_DEFINED_32
void Tundra::Internal::simd_copy_aligned_32_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    const uint8_t *dst_iter = (const uint8_t*)dst;

    constexpr uint8_t BYTE_WIDTH = 32;

    while(num_bytes >= BYTE_WIDTH)
    {
        __asm__ __volatile__
        (
            "vmovdqa (%0), %%ymm0\n\t"
            "vmovdqa %%ymm0, (%1)\n\t"
            :
            : "r" (src_iter), "r" (dst_iter)
            : "ymm0", "memory"
        );

        src_iter += BYTE_WIDTH;
        dst_iter += BYTE_WIDTH;
        num_bytes -= BYTE_WIDTH;
    }

    if(num_bytes == 0) return;

    // Handle remaining bytes with scalar copying.
    Tundra::Internal::scalar_copy_mem((void*)src_iter, (void*)dst_iter, 
        num_bytes);
}
#endif 

#ifndef TUNDRA_UNALIGNED_COPY_FUNCTION_DEFINED

#define TUNDRA_UNALIGNED_COPY_FUNCTION_DEFINED
void Tundra::Internal::simd_copy_unaligned_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    const uint8_t *dst_iter = (const uint8_t*)dst;

    constexpr uint8_t BYTE_WIDTH = 32;

    while(num_bytes >= BYTE_WIDTH)
    {
        __asm__ __volatile__
        (
            "vmovdqu (%0), %%ymm0\n\t"
            "vmovdqu %%ymm0, (%1)\n\t"
            :
            : "r"(src_iter), "r"(dst_iter)
            : "ymm0", "memory"
        );

        src_iter += BYTE_WIDTH;
        dst_iter += BYTE_WIDTH;
        num_bytes -= BYTE_WIDTH;
    }

    if(num_bytes == 0) return;

    // Handle remaining bytes with scalar copying.
    Tundra::Internal::scalar_copy_mem((void*)src_iter, (void*)dst_iter, 
        num_bytes);
}
#endif
#endif

// SSE2 instruction set supported.
#ifdef __SSE2__ 

#ifndef TUNDRA_SIMD_DEFINED_16 

#define TUNDRA_SIMD_DEFINED_16
void Tundra::Internal::simd_copy_aligned_16_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    const uint8_t *dst_iter = (const uint8_t*)dst;

    constexpr uint8_t BYTE_WIDTH = 16;

    while(num_bytes >= BYTE_WIDTH)
    {
        __asm__ __volatile__
        (
            "movdqa (%0), %%xmm0\n\t"
            "movdqa %%xmm0, (%1)\n\t"
            :
            : "r" (src_iter), "r" (dst_iter)
            : "xmm0", "memory"
        );

        src_iter += BYTE_WIDTH;
        dst_iter += BYTE_WIDTH;
        num_bytes -= BYTE_WIDTH;
    }

    if(num_bytes == 0) return;

    // Handle remaining bytes with scalar copying.
    Tundra::Internal::scalar_copy_mem((void*)src_iter, (void*)dst_iter, 
    num_bytes);
}
#endif // TUNDRA_SIMD_DEFINED_16 

#ifndef TUNDRA_UNALIGNED_COPY_FUNCTION_DEFINED 

#define TUNDRA_UNALIGNED_COPY_FUNCTION_DEFINED
void Tundra::Internal::simd_copy_unaligned_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    const uint8_t *dst_iter = (const uint8_t*)dst;

    constexpr uint8_t BYTE_WIDTH = 16;

    while(num_bytes >= BYTE_WIDTH)
    {
        __asm__ __volatile__
        (
            "movdqu (%0), %%xmm0\n\t"
            "movdqu %%xmm0, (%1)\n\t"
            :
            : "r" (src_iter), "r" (dst_iter)
            : "xmm0", "memory"
        );

        src_iter += BYTE_WIDTH;
        dst_iter += BYTE_WIDTH;
        num_bytes -= BYTE_WIDTH;
    }

    if(num_bytes == 0) return;

    // Handle remaining bytes with scalar copying.
    Tundra::Internal::scalar_copy_mem((void*)src_iter, (void*)dst_iter, 
    num_bytes);
}
#endif 
#endif 

// NEON instruction set supported.
#ifdef __ARM_NEON__

#ifdef __aarch64__

#ifndef TUNDRA_SIMD_DEFINED_16
#define TUNDRA_SIMD_DEFINED_16
void Tundra::Internal::simd_copy_aligned_16_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    const uint8_t *dst_iter = (const uint8_t*)dst;

    constexpr uint8_t BYTE_WIDTH = 16;

    while(num_bytes >= BYTE_WIDTH)
    {
        __asm__ __volatile__
        (
            "ld1 {v0.16b}, [%0]       \n\t"
            "st1 {v0.16b}, [%1]       \n\t"
            :
            : "r"(src_iter), "r"(dst_iter)
            : "v0", "memory"
        );

        src_iter += BYTE_WIDTH;
        dst_iter += BYTE_WIDTH;
        num_bytes -= BYTE_WIDTH;
    }

    if(num_bytes == 0) return;

    // Handle remaining bytes with scalar copying.
    Tundra::Internal::scalar_copy_mem((void*)src_iter, (void*)dst_iter, 
    num_bytes);
}
#endif

#ifndef TUNDRA_UNALIGNED_COPY_FUNCTION_DEFINED

#define TUNDRA_UNALIGNED_COPY_FUNCTION_DEFINED
void Tundra::Internal::simd_copy_unaligned_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    const uint8_t *dst_iter = (const uint8_t*)dst;

    constexpr uint8_t BYTE_WIDTH = 16;

    while(num_bytes >= BYTE_WIDTH)
    {
        __asm__ __volatile__
        (
            "ld1 {v0.16b}, [%[src]]\n"
            "st1 {v0.16b}, [%[dst]]\n"
            :
            : [src] "r" (src_iter), [dst] "r" (dst_iter)
            : "v0", "memory"
        );

        src_iter += BYTE_WIDTH;
        dst_iter += BYTE_WIDTH;
        num_bytes -= BYTE_WIDTH;
    }

    if(num_bytes == 0) return;

    // Handle remaining bytes with scalar copying.
    Tundra::Internal::scalar_copy_mem((void*)src_iter, (void*)dst_iter, 
        num_bytes);
}
#endif

// Old NEON instruction set.
else

#ifndef TUNDRA_SIMD_DEFINED_16

#define TUNDRA_SIMD_DEFINED_16
void Tundra::Internal::simd_copy_aligned_16_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    const uint8_t *dst_iter = (const uint8_t*)dst;

    constexpr uint8_t BYTE_WIDTH = 16;

    while(num_bytes >= BYTE_WIDTH)
    {
        __asm__ __volatile__
        (
            "vld1.8 {q0}, [%[src]]\n"
            "vst1.8 {q0}, [%[dst]]\n"
            :
            : [src] "r" (src_iter), [dst] "r" (dst_iter)
            : "q0", "memory"
        );

        src_iter += BYTE_WIDTH;
        dst_iter += BYTE_WIDTH;
        num_bytes -= BYTE_WIDTH;
    }

    if(num_bytes == 0) return;

    // Handle remaining bytes with scalar copying.
    Tundra::Internal::scalar_copy_mem((void*)src_iter, (void*)dst_iter, 
        num_bytes);
}
#endif
#endif
#endif 


// Public ----------------------------------------------------------------------

void Tundra::copy_mem(const void *src, void *dst, uint64_t num_bytes)
{
    // -- Check alignment and SIMD support --

    #ifdef TUNDRA_SIMD_DEFINED_32
    
    // If we are 32 byte aligned.
    if(((uintptr_t)src & 31) == 0 && ((uintptr_t)dst & 31) == 0)
    {
        Tundra::Internal::simd_copy_aligned_32_mem(src, dst, num_bytes);
        return;
    }
    #endif

    #ifdef TUNDRA_SIMD_DEFINED_16
    
    // If we are 16 byte aligned.
    if(((uintptr_t)src & 15) == 0 && ((uintptr_t)dst & 15) == 0)
    {
        Tundra::Internal::simd_copy_aligned_16_mem(src, dst, num_bytes);
        return;
    }
    #endif

    Tundra::Internal::scalar_copy_mem(src, dst, num_bytes);
}

void* Tundra::aligned_alloc(uint64_t alignment, uint64_t num_bytes)
{
    #ifdef _MSC_VER

    return _aligned_malloc(num_bytes, alignment);
    #else

    void* aligned_mem = NULL;
    posix_memalign(&aligned_mem, alignment, num_bytes);
    return aligned_mem;
    #endif
}

void Tundra::aligned_free(void *mem)
{
    #ifdef _MSC_VER
    _aligned_free(mem);
    #else
    free(mem);
    #endif
}

void* Tundra::alloc_and_copy_mem(const void *old_memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity)
{
    void *new_memory = malloc(new_byte_capacity);

    Tundra::copy_mem(old_memory, new_memory, num_copy_bytes);

    return new_memory;
}

uint64_t Tundra::reserve_memory(void **memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity)
{
    return Tundra::Internal::underlying_reserve_mem<0>(memory, 
        num_reserve_bytes, num_used_bytes, capacity);

    // uint64_t remaining_space = capacity - num_used_bytes;

    // // If the capacity is already sufficient 
    // if(num_reserve_bytes <= remaining_space) return capacity;

    // // Calculate the number of times the capacity needs to be doubled to at 
    // // least reach the new capacity.
    // uint8_t num_double = (uint8_t)ceil(log2((double)num_reserve_bytes / 
    //     capacity));

    // // Continuously double the new capacity until it's greater than the reserved
    // // bytes.
    // uint64_t new_capacity = capacity * (1ULL << num_double);

    // void* new_memory = Tundra::alloc_and_copy_mem(*memory, num_used_bytes, 
    //     new_capacity);
    
    // free(*memory);

    // *memory = new_memory;

    // return new_capacity;
}
