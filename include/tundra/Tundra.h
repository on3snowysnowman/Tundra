/**
 * @file Tundra.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Entry to the library, provides init method.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_H
#define TUNDRA_H

#include "tundra/utils/SystemInfo.h"
#include "tundra/internal/MemAllocHandler.h"
#include "tundra/utils/FatalHandler.h"

#ifndef TUNDRA_NOLIBC
#include <unistd.h> 
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes any components in the library that need initializing. Must
 * be called before the library is used.
 * 
 * @return int 0 if successful. Error code otherwise.
 */
static inline int Tundra_init()
{
    // -- Initialize page size -- 

    // Libc is linked, use sysconf to get the page size as it was not manually
    // set during custom entry point startup.
    #ifndef TUNDRA_NOLIBC

    #ifdef TUNDRA_PLATFORM_LINUX

    #ifdef TUNDRA_SYS_x86_64

    InTundra_Mem_data_instance.page_size_bytes = sysconf(_SC_PAGESIZE);    

    #else // Linux and not x86-64
    #error Not implemented.
    #endif // TUNDRA_SYS_x86_64

    #else // Windows / Apple
    #error Not implemented.
    #endif // TUNDRA_PLATFORM_LINUX

    #endif // TUNDRA_NOLIBC

    if(InTundra_Mem_data_instance.page_size_bytes == -1)
    {
        TUNDRA_FATAL("Failed to get page size.");
    }

    // -- --

    InTundra_Mem_init();
    return 0;
}

/**
 * @brief Shuts down any components in the library that need shutting down.
 * Must be called when the library is no longer needed.
 * 
 */
static inline int Tundra_shutdown()
{
    InTundra_Mem_shutdown();
    return 0;
}

#ifdef __cplusplus
}   
#endif

#endif