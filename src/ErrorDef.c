/**
 * @file ErrorDef.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Error definitions and converting them to a readable message.
 * @version 0.1
 * @date 2026-05-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tundra/common/TypeDef.h"
#include "tundra/common/ErrorDef.h"

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

 const char* Tundra_err_to_rdbl(i64 error)
{
    if(error < 0) error = -error;

    switch(error)
    {
        case TUNDRA_EPERM: return "Operation not permitted";

        case TUNDRA_ENOENT: return "No such file or directory";

        case TUNDRA_EINTR: return "Interrupted syscall";

        case TUNDRA_EIO: return "I/O Error";
        
        case TUNDRA_EBADF: return "Bad file descriptor";

        case TUNDRA_EAGAIN: return "Try again / would block";

        case TUNDRA_ENOMEM: return "Out of memory";

        case TUNDRA_EACCES: return "Permission denied";

        case TUNDRA_EFAULT: return "Bad address";

        case TUNDRA_EINVAL: return "Invalid argument";

        default: return "Unkown Error";
    }
}

#else // TUNDRA_SYS_x86_64
#error Not Implemented
#endif // TUNDRA_SYS_x86_64

#else // TUNDRA_PLATFORM_LINUX
#error Not Implemented
#endif // TUNDRA_PLATFORM_LINUX
