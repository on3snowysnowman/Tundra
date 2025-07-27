/**
 * @file MemoryUtils.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for performing operations on memory.
 * @version 0.1
 * @date 07-26-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "../include/tundra/utils/memory/MemoryUtils.hpp"
#include "tundra/utils/memory/MemoryAlloc.hpp"


// Internal -------------------------------------------------------------------

bool Tundra::Internal::is_address_aligned(uintptr_t ptr, uint8_t alignment)
{
    return (ptr & (alignment - 1)) == 0;
}

void Tundra::Internal::scalar_copy_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (uint8_t*)src;
    uint8_t *dst_iter = (uint8_t*)dst;

    while(num_bytes > 0)
    {
        *dst_iter = *src_iter;
        ++src_iter;
        ++dst_iter;
        --num_bytes;
    }
}

void Tundra::Internal::scalar_copy_aligned_2_mem(const void *src, 
    void *dst, uint64_t num_bytes)
{
    const uint16_t *src_iter = (uint16_t*)src;
    uint16_t *dst_iter = (uint16_t*)dst;

    static constexpr uint8_t STRIDE = 2;

    while(num_bytes >= STRIDE)
    {
        *dst_iter = *src_iter;
        ++src_iter;
        ++dst_iter;
        num_bytes -= STRIDE;
    }

    if(num_bytes == 0) { return; }

    scalar_copy_mem((const void*)src_iter, (void*)dst_iter, 
        num_bytes);
}

void Tundra::Internal::scalar_copy_aligned_4_mem(const void *src, 
    void *dst, uint64_t num_bytes)
{
    const uint32_t *src_iter = (uint32_t*)src;
    uint32_t *dst_iter = (uint32_t*)dst;

    static constexpr uint8_t STRIDE = 4;

    while(num_bytes >= STRIDE)
    {
        *dst_iter = *src_iter;
        ++src_iter;
        ++dst_iter;
        num_bytes -= STRIDE;
    }

    if(num_bytes == 0) { return; }

    scalar_copy_aligned_2_mem((const void*)src_iter, (uint16_t*)dst_iter, 
        num_bytes);
}

void Tundra::Internal::scalar_copy_aligned_8_mem(const void *src, 
    void *dst, uint64_t num_bytes)
{
    const uint64_t *src_iter = (uint64_t*)src;
    uint64_t *dst_iter = (uint64_t*)dst;

    static constexpr uint8_t STRIDE = 8;

    while(num_bytes >= STRIDE)
    {
        *dst_iter = *src_iter;
        ++src_iter;
        ++dst_iter;
        num_bytes -= STRIDE;
    }

    if(num_bytes == 0) { return; }

    scalar_copy_aligned_4_mem((const void*)src_iter, (void*)dst_iter, 
        num_bytes);
}

// AVX2 instruction set supported. 
#ifdef __AVX2__ 

#ifndef TUNDRA_SIMD_DEFINED_32

#define TUNDRA_SIMD_DEFINED_32

void Tundra::Internal::simd_copy_aligned_32_mem(const void *src, void *dst, 
    uint64_t num_bytes)
{
    const uint8_t *src_iter = (const uint8_t*)src;
    uint8_t *dst_iter = (uint8_t*)dst;

    static constexpr uint8_t BYTE_WIDTH = 32;

    static constexpr uint16_t LOOP_UNROLLING_STRIDE = 32 * 8;

    // Chunk through 8 sets of 32 bytes. 
    while(num_bytes >= LOOP_UNROLLING_STRIDE)
    {
         __asm__ __volatile__
        (
            "prefetcht0 1024(%0)\n\t"
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
    Tundra::Internal::scalar_copy_aligned_8_mem((const uint64_t*)src_iter, 
        (uint64_t*)dst_iter, num_bytes);
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
    Tundra::Internal::scalar_copy_mem((uint8_t*)src_iter, (uint8_t*)dst_iter, 
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
    Tundra::Internal::scalar_copy_aligned_8_mem((const uint64_t*)src_iter, 
        (uint64_t*)dst_iter, num_bytes);
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

// Public ----------------------------------------------------------------------

void Tundra::copy_mem(const void *src, void *dst, uint64_t num_bytes)
{
    // rep movsb is fast on modern machines. 
    // TODO: Implement fallback for older machines without erms. 
    asm volatile("rep movsb"
                : "=D"(dst), "=S"(src), "=c"(num_bytes)
                : "0"(dst), "1"(src), "2"(num_bytes)
                : "memory");
    return;

    // // Get the maximum alignment (power of 2) supported by both addresses. This
    // // alignment index will be used as an index into the alignment dispatch
    // // function table.
    // uint32_t align_pow2 = Tundra::Internal::get_num_trailing_zeros(
    //     (uintptr_t)src | (uintptr_t)dst);

    // // If maximum supported alignment is greater than the greatest alignment we
    // // support, fall back to the largest supported alignment.
    // align_pow2 = (align_pow2 > TUNDRA_ALIGN_DISP_ARR_LARGEST_INDEX) ? 
    //     TUNDRA_ALIGN_DISP_ARR_LARGEST_INDEX : align_pow2;

    // Tundra::Internal::alignment_dispatch_table[align_pow2](src, dst, num_bytes);
    // return;

    
    // // -- Check alignment and SIMD support --

    // #ifdef TUNDRA_SIMD_DEFINED_32
    
    // // If we are 32 byte aligned.
    // if(num_bytes >= 32 && 
    //     Tundra::Internal::is_address_aligned((uintptr_t)src, 32) &&
    //     Tundra::Internal::is_address_aligned((uintptr_t)dst, 32))
    // {
    //     Tundra::Internal::simd_copy_aligned_32_mem(src, dst, num_bytes);
    //     return;
    // }
    // #endif

    // #ifdef TUNDRA_SIMD_DEFINED_16
    
    // // If we are 16 byte aligned.
    // if(num_bytes >= 16 &&
    //     Tundra::Internal::is_address_aligned((uintptr_t)src, 16) &&
    //     Tundra::Internal::is_address_aligned((uintptr_t)dst, 16))
    // {
    //     Tundra::Internal::simd_copy_aligned_16_mem(src, dst, num_bytes);
    //     return;
    // }
    // #endif

    // if(num_bytes >= 8 && 
    //     Tundra::Internal::is_address_aligned((uintptr_t)src, 8) &&
    //     Tundra::Internal::is_address_aligned((uintptr_t)dst, 8))
    // {
    //     Tundra::Internal::scalar_copy_aligned_8_mem((const uint64_t*)src,
    //         (uint64_t*)dst, num_bytes);
    //     return;
    // }

    // else if(Tundra::Internal::is_address_aligned((uintptr_t)src, 4) &&
    //     Tundra::Internal::is_address_aligned((uintptr_t)dst, 4))
    // {
    //     Tundra::Internal::scalar_copy_aligned_4_mem((const uint32_t*)src,
    //         (uint32_t*)dst, num_bytes);
    //     return;
    // }

    // else if(Tundra::Internal::is_address_aligned((uintptr_t)src, 2) &&
    //     Tundra::Internal::is_address_aligned((uintptr_t)dst, 2))
    // {
    //     Tundra::Internal::scalar_copy_aligned_2_mem((const uint16_t*)src,
    //         (uint16_t*)dst, num_bytes);
    //     return;
    // }

    // Tundra::Internal::scalar_copy_mem((const uint8_t*)src, (uint8_t*)dst, 
    //     num_bytes);
}

void Tundra::erase_and_shift_bytes(void *memory, uint64_t index, 
    uint64_t num_erase_bytes, uint64_t total_bytes)
{
    uint64_t src_position = index + num_erase_bytes;

    Tundra::copy_mem((uint8_t*)memory + src_position, 
        (uint8_t*)memory + index, total_bytes - src_position);
}