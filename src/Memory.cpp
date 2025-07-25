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

// Internal --------------------------------------------------------------------

void Tundra::Internal::scalar_copy_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    uint8_t *src_iter = (uint8_t*)src;
    uint8_t *dst_iter = (uint8_t*)dst;

    static constexpr uint8_t EIGHT_BIT_INCREMENT = 8;

    while(num_bytes >= EIGHT_BIT_INCREMENT)
    {
        *((uint64_t*)dst_iter) = *((uint64_t*)src_iter);
        src_iter += EIGHT_BIT_INCREMENT;
        dst_iter += EIGHT_BIT_INCREMENT;
        num_bytes -= EIGHT_BIT_INCREMENT;
    }

    static constexpr uint8_t FOUR_BIT_INCREMENT = 4;

    while(num_bytes >= FOUR_BIT_INCREMENT)
    {
        *((uint32_t*)dst_iter) = *((uint32_t*)src_iter);
        src_iter += FOUR_BIT_INCREMENT;
        dst_iter += FOUR_BIT_INCREMENT;
        num_bytes -= FOUR_BIT_INCREMENT;
    }

    static constexpr uint8_t TWO_BIT_INCREMENT = 2;

    while(num_bytes >= TWO_BIT_INCREMENT)
    {
        *((uint16_t*)dst_iter) = *((uint16_t*)src_iter);
        src_iter += TWO_BIT_INCREMENT;
        dst_iter += TWO_BIT_INCREMENT;
        num_bytes -= TWO_BIT_INCREMENT;
    }

    while(num_bytes > 0)
    {
        *dst_iter = *src_iter;
        ++src_iter;
        ++dst_iter;
        --num_bytes;
    }
}

// AVX2 instruction set supported. 
#ifdef __AVX2__ 

#ifndef TUNDRA_SIMD_DEFINED_32

#define TUNDRA_SIMD_DEFINED_32
// void Tundra::Internal::simd_copy_large_aligned_32_mem(const void *src, 
//     void *dst, uint64_t num_bytes)
// {
//     __asm__ volatile (
//         "rep movsb"
//         : "=D" (dst), "=S" (src), "=c" (num_bytes)
//         : "0" (dst), "1" (src), "2" (num_bytes)
//         : "memory"
//     );

//     OR

//     const uint8_t* src_iter = (const uint8_t*)src;
//     uint8_t* dst_iter = (uint8_t*)dst;

//     static constexpr uint8_t BYTE_WIDTH = 32;

//     while(num_bytes >= BYTE_WIDTH)
//     {
//         __asm__ __volatile__
//         (
//             "vmovdqu (%0), %%ymm0\n\t"
//             "vmovntdq %%ymm0, (%1)\n\t" // Store to the streaming WC buffer.
//             :
//             : "r" (src_iter), "r" (dst_iter)
//             : "ymm0", "memory"
//         );

//         src_iter += BYTE_WIDTH;
//         dst_iter += BYTE_WIDTH;
//         num_bytes -= BYTE_WIDTH;
//     }

//     // Flush the WC buffer to RAM.
//     __asm__ __volatile__("sfence" ::: "memory");

//     if(num_bytes == 0) { return; }

//     // Handle remaining bytes with scalar fallback
//     Tundra::Internal::scalar_copy_mem((void*)src_iter, 
//         (void*)dst_iter, num_bytes);
// }

void Tundra::Internal::simd_copy_aligned_32_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    // if(num_bytes > 6400)
    // {
    //     Tundra::Internal::simd_copy_large_aligned_32_mem(src, dst, num_bytes);
    //     return;
    // }

    const uint8_t *src_iter = (const uint8_t*)src;
    uint8_t *dst_iter = (uint8_t*)dst;

    static constexpr uint8_t BYTE_WIDTH = 32;

    static constexpr uint16_t LOOP_UNROLLING_STRIDE = 32 * 8;

    // Chunk through 8 sets of 32 bytes. 
    while(num_bytes >= LOOP_UNROLLING_STRIDE)
    {
         __asm__ __volatile__
        (
            "prefetcht0 512(%0)\n\t"
            "vmovdqa   (%0), %%ymm0\n\t"
            "vmovdqa 32(%0), %%ymm1\n\t"
            "vmovdqa 64(%0), %%ymm2\n\t"
            "vmovdqa 96(%0), %%ymm3\n\t"
            "vmovdqa 128(%0), %%ymm4\n\t"
            "vmovdqa 160(%0), %%ymm5\n\t"
            "vmovdqa 192(%0), %%ymm6\n\t"
            "vmovdqa 224(%0), %%ymm7\n\t"

            "vmovdqa %%ymm0,   (%1)\n\t"
            "vmovdqa %%ymm1, 32(%1)\n\t"
            "vmovdqa %%ymm2, 64(%1)\n\t"
            "vmovdqa %%ymm3, 96(%1)\n\t"
            "vmovdqa %%ymm4, 128(%1)\n\t"
            "vmovdqa %%ymm5, 160(%1)\n\t"
            "vmovdqa %%ymm6, 192(%1)\n\t"
            "vmovdqa %%ymm7, 224(%1)\n\t"
            :
            : "r" (src_iter), "r" (dst_iter)
            : "memory", "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6",
              "ymm7"
        );

        src_iter += LOOP_UNROLLING_STRIDE;
        dst_iter += LOOP_UNROLLING_STRIDE;
        num_bytes -= LOOP_UNROLLING_STRIDE;
    }    

    __asm__ __volatile__
    (
        "prefetcht0 256(%0)\n\t"
        : 
        : "r" (src_iter)
    );

    // Fetch the remaining bytes in individual increments.
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

    if(num_bytes == 0) { return; }

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

    static constexpr uint8_t BYTE_WIDTH = 32;

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

    static constexpr uint8_t BYTE_WIDTH = 16;

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

    static constexpr uint8_t BYTE_WIDTH = 16;

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

    static constexpr uint8_t BYTE_WIDTH = 16;

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

    static constexpr uint8_t BYTE_WIDTH = 16;

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

    static constexpr uint8_t BYTE_WIDTH = 16;

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

uint32_t Tundra::Internal::get_num_trailing_zeros(uint64_t bits)
{
    uint64_t num_trailing;
 
    #ifdef __x86_64__
    
    __asm__ __volatile__
    (
        "bsr %1, %0\n\t"         // Find index of MSB.
        "xor $63, %0\n\t"        // Convert to count of leading zeros.
        "cmovz %1, %0"           // If input was zero, result = 64.
        : "=&r"(num_trailing)
        : "r"(bits)
        : "cc"
    );
    
    #else

    __asm__ __volatile__
    (
        "clz %0, %1"
        : "=r"(result)
        : "r"(x)
    );

    #endif

    return (uint32_t)num_trailing;
}

uint64_t Tundra::Internal::calc_new_capacity_by_doubling(
    uint64_t required_bytes, uint64_t capacity)
{
    // Calculate how many full capacity units are needed to fit the required 
    // bytes. This is a ceiling division: round up (required_bytes / capacity).
    uint64_t overfill_ratio = 
        (required_bytes + capacity - 1) / capacity;
    
    // Find the position of the most significant set bit in the overfill ratio.
    // This is equivalent to floor(log2(overfill_ratio)), which tells us how
    // many doublings are needed to just reach the ratio.
    uint8_t msb_position = 63 - get_num_trailing_zeros(overfill_ratio); // NOLINT

    // If the ratio is not already a power of 2, we need to round up,
    // so we increment the position to get ceil(log2(overfill_ratio)).
    msb_position = ((overfill_ratio & (overfill_ratio - 1)) == 0) ? 
        msb_position : msb_position + 1;
        
    // Double the capacity the required number of times to ensure it's large
    // enough to hold the requested number of bytes.
    return capacity << msb_position;
}


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

void Tundra::aligned_free(void *mem)
{
    #ifdef _WIN32
    _aligned_free(mem);
    #else
    free(mem);
    #endif
}

void* Tundra::alloc_and_copy_mem(const void *old_memory, uint64_t num_copy_bytes, 
    uint64_t new_byte_capacity)
{
    void *new_memory = malloc(new_byte_capacity);

    if(!(bool)new_memory) return NULL;

    Tundra::copy_mem(old_memory, new_memory, num_copy_bytes);

    return new_memory;
}

uint64_t Tundra::reserve_mem(void **memory, uint64_t num_reserve_bytes, 
    uint64_t num_used_bytes, uint64_t capacity)
{
    return Tundra::Internal::underlying_reserve_mem<0>(memory, 
        num_reserve_bytes, num_used_bytes, capacity);
}
