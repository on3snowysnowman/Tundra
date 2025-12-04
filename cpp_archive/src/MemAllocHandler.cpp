/**
 * @file MemAllocHandler.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory that is smaller than 
 * or equal to 4096 Bytes.
 * @version 0.1
 * @date 2025-10-23
 * 
 * @copyright Copyright (c) 2025
*/

#include "tundra/internal/MemAllocHandler.hpp"
#include "tundra/internal/LargeMemAlloc.hpp"
#include "tundra/internal/SmallMemAlloc.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/SystemInfo.hpp"

#ifdef TUNDRA_USE_C_MALLOC
#include <cstdlib>
#endif

#ifdef TUNDRA_PLATFORM_LINUX // ------------------------------------------------

#include <unistd.h> // Temporary
// #include <sys/mman.h>

#ifndef PROT_READ
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#endif
#ifndef MAP_PRIVATE
#define MAP_PRIVATE   0x02
#define MAP_ANONYMOUS 0x20
#endif

// Linux x86-64 syscall numbers
static constexpr long SYS_MMAP   = 9;
static constexpr long SYS_MUNMAP = 11;


#ifdef TUNDRA_SYS_x86_64 // ----------------------------------------------------

/**
 * @brief Inline assembly call to mmap on x86_64 linux systems.
 * 
 * @param addr 
 * @param length 
 * @param prot 
 * @param flags 
 * @param fd 
 * @param offset 
 *
 * @return void* Pointer to the allocated bytes, or -errno on failure. 
 */
static void* mmap_syscall(void *addr, size_t length, int prot, int flags, 
    int fd, long offset)
{
    register long r10 __asm__("r10") = (long)flags;
    register long r8  __asm__("r8")  = (long)fd;
    register long r9  __asm__("r9")  = (long)offset;

    register long rax __asm__("rax") = SYS_MMAP;
    register void* rdi __asm__("rdi") = addr;
    register size_t rsi __asm__("rsi") = length;
    register long rdx __asm__("rdx") = (long)prot;

    __asm__ volatile (
        "syscall"
        : "+a"(rax)                       
        : "D"(rdi), "S"(rsi), "d"(rdx), 
          "r"(r10), "r"(r8), "r"(r9)    
        : "rcx", "r11", "memory"
    );

    // On success rax is a pointer, on error rax is -errno
    return reinterpret_cast<void*>(rax);
}

/**
 * @brief Inline assembly call to munmap on x86_64 linux systems.
 * 
 * @param addr 
 * @param num_bytes 
 *
 * @return long 0 on success, -errno on failure.
 */
static long munmap_syscall(void *addr, Tundra::uint64 num_bytes)
{
    register long rax __asm__("rax") = SYS_MUNMAP;
    register void* rdi __asm__("rdi") = addr;
    register size_t rsi __asm__("rsi") = num_bytes;

    __asm__ volatile (
        "syscall"
        : "+a"(rax)
        : "D"(rdi), "S"(rsi)
        : "rcx", "r11", "memory"
    );

    // 0 if success, otherwise the error number is returned.
    return rax;
}

#endif // ----------------------------------------------------------------------

#else // ARM, Windows / Apple / Linux
#error Implement this.
#endif // ----------------------------------------------------------------------


// Define static variable.
Tundra::uint64 Tundra::Internal::Mem::SystemMemData::page_size_bytes;


void Tundra::Internal::Mem::init()
{
    #ifdef TUNDRA_PLATFORM_LINUX
    long page_size = sysconf(_SC_PAGE_SIZE); // Temporary.

    if(page_size == -1)
    {
        TUNDRA_FATAL("Failed to get page size.");
    }

    Tundra::Internal::Mem::SystemMemData::page_size_bytes = 
        (Tundra::uint64)page_size;
    
    #else // Windows.
    #error Implement this.
    #endif

    // Initialize the allocators.
    Tundra::Internal::Mem::SmallAlloc::init();
    Tundra::Internal::Mem::LargeAlloc::init();
}

void Tundra::Internal::Mem::free(void *ptr)
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::free(ptr);
    #endif

    if(ptr == nullptr)
    {
        return;
    }

    if(Tundra::Internal::Mem::SmallAlloc::is_ptr_in_arena(ptr))
    {
        Tundra::Internal::Mem::SmallAlloc::free(ptr);
        return;
    }

    Tundra::Internal::Mem::LargeAlloc::free(ptr);
    
}

void* Tundra::Internal::Mem::malloc(Tundra::uint64 num_bytes)
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::malloc(num_bytes);
    #endif

    if(num_bytes == 0) 
    {
        TUNDRA_FATAL("Requested allocation of 0 bytes.");
    }

    return (num_bytes > 
        Tundra::Internal::Mem::SmallAlloc::MAX_SIZE_CLASS_BYTE_SIZE) ? 
        Tundra::Internal::Mem::LargeAlloc::malloc(num_bytes) : 
        Tundra::Internal::Mem::SmallAlloc::malloc(num_bytes);
}

void Tundra::Internal::Mem::release_mem_to_os(void *ptr, 
    Tundra::uint64 num_bytes)
{
    if(num_bytes % SystemMemData::page_size_bytes != 0)
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

void* Tundra::Internal::Mem::get_mem_from_os(Tundra::uint64 num_bytes)
{
    // Ensure that the number of bytes is an increment of the page size.
    if(num_bytes % SystemMemData::page_size_bytes != 0)
    {
        TUNDRA_FATAL("Requested bytes were not an increment of the system's "
            "memory page size.");
    }

    void *mem = nullptr;

    #ifdef TUNDRA_PLATFORM_LINUX

    mem = mmap_syscall(nullptr, num_bytes, PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(reinterpret_cast<long>(mem) < 0)
    {
        TUNDRA_FATAL("mmap syscall failed with error: %d.", 
            reinterpret_cast<long>(mem));
    }

    #else
    #error Implement this.
    #endif

    return mem;
}
