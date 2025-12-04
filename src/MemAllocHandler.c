/**
 * @file MemAllocHandler.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory that is smaller than 
 * or equal to 4096 Bytes.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/

#include "tundra/internal/MemAllocHandler.h"
#include "tundra/utils/SystemInfo.h"
#include "tundra/utils/FatalHandler.h"
#include "tundra/internal/SmallMemAlloc.h"

#ifdef TUNDRA_USE_C_MALLOC
#include <stdlib.h>
#endif

#ifdef TUNDRA_PLATFORM_LINUX // ------------------------------------------------

#include <unistd.h> // Temporary

#ifndef PROT_READ
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#endif
#ifndef MAP_PRIVATE
#define MAP_PRIVATE   0x02
#define MAP_ANONYMOUS 0x20
#endif

// Linux x86-64 syscall numbers
#define SYS_MMAP 9;
#define SYS_MUNMAP 11;

#ifdef TUNDRA_SYS_x86_64 // ----------------------------------------------------    

static void* mmap_syscall(void *addr, long long length, int prot, int flags, 
    int fd, long offset)
{
    register long r10 __asm__("r10") = (long)flags;
    register long r8  __asm__("r8")  = (long)fd;
    register long r9  __asm__("r9")  = (long)offset;

    register long rax __asm__("rax") = SYS_MMAP;
    register void* rdi __asm__("rdi") = addr;
    register long long rsi __asm__("rsi") = length;
    register int rdx __asm__("rdx") = prot;

    __asm__ volatile (
        "syscall"
        : "+a"(rax)
        : "D"(rdi), "S"(rsi), "d"(rdx), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory"
    );

    // Pointer to allocated memory if success, otherwise the error number is 
    // returned.
    return (void*)rax;
}

static long munmap_syscall(void *addr, long long num_bytes)
{
    register long rax __asm__("rax") = SYS_MUNMAP;
    register void* rdi __asm__("rdi") = addr;
    register long long rsi __asm__("rsi") = num_bytes;

    __asm__ volatile (
        "syscall"
        : "+a"(rax)
        : "D"(rdi), "S"(rsi)
        : "rcx", "r11", "memory"
    );

    // 0 if success, otherwise the error number is returned.
    return rax;
}

#else 
#error Implement this.
#endif // TUNDRA_SYS_x86_64 ----------------------------------------------------

#else // ARM, Windows / Apple / Linux
#error Implement this.
#endif // TUNDRA_PLATFORM_LINUX ------------------------------------------------

// Global instance
TundraIn_SystemMemData TundraIn_Mem_data_instance = {0};

void TundraIn_Mem_init()
{
    #ifdef TUNDRA_PLATFORM_LINUX

    long page_size = sysconf(_SC_PAGESIZE);

    if(page_size == -1)
    {
        TUNDRA_FATAL("Failed to get page size.");
    }

    TundraIn_Mem_data_instance.page_size_bytes = (uint64)page_size;

    #else // ARM, Windows / Apple
    #error Implement this.
    #endif

    // Initialize allocators
    TundraIn_SmlMemAlc_init();
}

void TundraIn_Mem_free(void *ptr) 
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::free(ptr);
    #endif

    if(ptr == NULL) { return; }

    if(TundraIn_SmlMemAlc_is_ptr_in_arena(ptr))
    {
        TundraIn_SmlMemAlc_free(ptr);
        return;
    }

    // Large allocation
    // To be implemented
}

void* TundraIn_Mem_malloc(uint64 num_bytes) 
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::malloc(num_bytes);
    #endif

    if(num_bytes == 0) 
    {
        TUNDRA_FATAL("Requested allocation of 0 bytes.");
    }

    return (num_bytes > 
        TUNDRA_MAX_SIZE_CLASS_BYTE_SIZE) ? 
        Tundra::Internal::Mem::LargeAlloc::malloc(num_bytes) : 
        Tundra::Internal::Mem::SmallAlloc::malloc(num_bytes);
}

void TundraIn_Mem_release_mem_to_os(void *ptr, uint64 num_bytes) {}

void *TundraIn_Mem_get_mem_from_os(uint64 num_bytes) {}

