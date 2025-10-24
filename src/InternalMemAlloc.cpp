/**
 * @file SmallMemAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory that is smaller than 
 * or equal to 4096 Bytes.
 * @version 0.1
 * @date 2025-10-23
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "tundra/internal/InternalMemAlloc.hpp"
#include "tundra/internal/SmallMemAlloc.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/internal/SystemCheck.hpp"


#ifdef TUNDRA_PLATFORM_POSIX
#include <unistd.h>
#include <sys/mman.h>

#else // Windows
#include <windows.h>

#endif

#ifdef TUNDRA_USE_C_MALLOC
#include <cstdlib>
#endif

// Define static variable.
Tundra::uint64 Tundra::Internal::Mem::SystemMemData::page_size_bytes;


void Tundra::Internal::Mem::init()
{
    #ifdef TUNDRA_PLATFORM_POSIX
    long page_size = sysconf(_SC_PAGE_SIZE);

    if(page_size == -1)
    {
        TUNDRA_FATAL("Failed to get page size.");
    }

    Tundra::Internal::Mem::SystemMemData::page_size_bytes = 
        (Tundra::uint64)page_size;
    
    #else // Windows.
    #error Implement this.
    #endif

    // Initialize the small allocator.
    Tundra::Internal::Mem::SmallAlloc::init();
}

void Tundra::Internal::Mem::free(void *ptr)
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::free(ptr);
    #endif

    // Free with only the small allocator, temporary.
    Tundra::Internal::Mem::SmallAlloc::free(ptr);
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

    if(num_bytes > Tundra::Internal::Mem::SmallAlloc::MAX_SIZE_CLASS_BYTE_SIZE)
    {
        // Temporary.
        TUNDRA_FATAL("Requested bytes are too large to be represented by the "
            "small allocator.");
    }

    // Malloc with the small allocator.
    return Tundra::Internal::Mem::SmallAlloc::malloc(num_bytes);
}

void Tundra::Internal::Mem::release_to_os(void *ptr, Tundra::uint64 num_bytes)
{
    #ifdef TUNDRA_PLATFORM_POSIX

    if(munmap(ptr, num_bytes) != 0)
    {
        TUNDRA_FATAL("munmap failed.");
    }

    #else
    #error Implement this.
    #endif
}

void* Tundra::Internal::Mem::get_mem_from_os(Tundra::uint64 num_bytes)
{
    void *mem = nullptr;

    #ifdef TUNDRA_PLATFORM_POSIX
    mem = mmap(nullptr, num_bytes, PROT_READ|PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(mem == MAP_FAILED)
    {
        TUNDRA_FATAL("mmap failed.");
    }

    #else
    #error Implement this.
    #endif

    return mem;
}
