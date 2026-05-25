/**
 * @file IOInterface.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for interacting with the OS for intput and output.
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/internal/IOInterface.h"
#include "tundra/internal/Syscall.h"

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

i64 InTundra_write_bytes(TundraIOHandle handle, const void *bytes, 
    u64 num_bytes)
{
    if (bytes == NULL) return -TUNDRA_EFAULT;
    if(num_bytes == 0) return 0;

    return InTundra_syscall(TUNDRA_LINUX_SYSCALL_WRITE, handle, (i64)bytes, 
        (i64)num_bytes);
}

#else // TUNDRA_SYS_x86_64
#error Not Implemented
#endif // TUNDRA_SYS_x86_64

#else // TUNDRA_PLATFORM_LINUX
#error Not Implemented
#endif // TUNDRA_PLATFORM_LINUX




