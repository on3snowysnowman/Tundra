/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"
#include "tundra/containers/LinkedListint.h"

int main(void)
{
    if(Tundra_init() != 0)
    {
        return 2;
    }

    if(Tundra_shutdown() != 0)
    {
        return 2;
    }

    return 0;
}

#ifdef TUNDRA_NOLIBC

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

void _start(void); // Silence missing prototype warning
void _start()
{
    __asm__ volatile (
        "mov %rsp, %rdi\n"  
        "call main\n"
        "mov %eax, %edi\n"   // exit status in edi
        "mov $60,  %eax\n"   // SYS_exit
        "syscall\n"
    );
}

#else // Linux and not x86-64
#error Not implemented.
#endif // TUNDRA_SYS_x86_64

#else // Windows / Apple
#error Not implemented.
#endif // TUNDRA_PLATFORM_LINUX

#endif // TUNDRA_NOLIBC
