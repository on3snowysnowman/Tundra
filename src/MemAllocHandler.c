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
#include "tundra/internal/LargeMemAlloc.h"


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
InTundra_SystemMemData InTundra_Mem_data_instance = {0};

void InTundra_Mem_init()
{
    #ifdef TUNDRA_PLATFORM_LINUX

    long page_size = sysconf(_SC_PAGESIZE);

    if(page_size == -1)
    {
        TUNDRA_FATAL("Failed to get page size.");
    }

    InTundra_Mem_data_instance.page_size_bytes = (uint64)page_size;

    #else // ARM, Windows / Apple
    #error Implement this.
    #endif

    // Initialize allocators
    InTundra_SmlMemAlc_init();
}

void InTundra_Mem_free(void *ptr) 
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::free(ptr);
    #endif

    if(ptr == NULL) { return; }

    if(InTundra_SmlMemAlc_is_ptr_in_arena(ptr))
    {
        InTundra_SmlMemAlc_free(ptr);
        return;
    }

    // Large allocation
    // To be implemented
}

void* InTundra_Mem_malloc(uint64 num_bytes) 
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
        InTundra_LgMemAlc_malloc(num_bytes) : 
        InTundra_SmlMemAlc_malloc(num_bytes);
}

void InTundra_Mem_release_mem_to_os(void *ptr, uint64 num_bytes)
{
    if(num_bytes % InTundra_Mem_data_instance.page_size_bytes != 0)
    {
        TUNDRA_FATAL("Byte size to free is not an increment of the system's "
            "memory page size.");
    }

    #ifdef TUNDRA_PLATFORM_LINUX

    long ret_value = munmap_syscall(ptr, num_bytes);

    if(ret_value != 0)
    {
        TUNDRA_FATAL("munmap syscall failed with error: %ld", -ret_value);
    }

    #else // Windows / Apple
    #error Implement this.
    #endif
}

void *InTundra_Mem_get_mem_from_os(uint64 num_bytes)
{
    // Ensure that the number of bytes is an increment of the page size.
    if(num_bytes % InTundra_Mem_data_instance.page_size_bytes != 0)
    {
        TUNDRA_FATAL("Requested bytes were not an increment of the system's "
            "memory page size.");
    }

    void *mem = NULL;

    #ifdef TUNDRA_PLATFORM_LINUX

    mem = mmap_syscall(NULL, num_bytes, PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if((long)mem < 0)
    {
        TUNDRA_FATAL("mmap syscall failed with error: %d.", (long)mem);
    }

    #else
    #error Implement this.
    #endif

    return mem;
}

