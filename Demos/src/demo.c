/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"
#include "tundra/utils/ConsoleOutput.h"
#include "tundra/utils/NumLimits.h"

#include <stdio.h>

int main(void)
{
    if (Tundra_init() != 0)
    {
        return 2;
    }

    Tundra_stdout_i8(TUNDRA_INT8_MIN);
    Tundra_stdout_char('\n');
    Tundra_stdout_u8(TUNDRA_UINT8_MAX);
    Tundra_stdout_char('\n');
    Tundra_stdout_i16(TUNDRA_INT16_MIN);
    Tundra_stdout_char('\n');
    Tundra_stdout_u16(TUNDRA_UINT16_MAX);
    Tundra_stdout_char('\n');
    Tundra_stdout_int(TUNDRA_INT32_MIN);
    Tundra_stdout_char('\n');
    Tundra_stdout_u32(TUNDRA_UINT32_MAX);
    Tundra_stdout_char('\n');
    Tundra_stdout_i64(TUNDRA_INT64_MIN);
    Tundra_stdout_char('\n');
    Tundra_stdout_u64(TUNDRA_UINT64_MAX);
    Tundra_stdout_char('\n');

    if (Tundra_shutdown() != 0)
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
    __asm__ volatile(
        "mov %rsp, %rdi\n"
        "call main\n"
        "mov %eax, %edi\n" // exit status in edi
        "mov $60,  %eax\n" // SYS_exit
        "syscall\n");
}

#else // Linux and not x86-64
#error Not implemented.
#endif // TUNDRA_SYS_x86_64

#else // Windows / Apple
#error Not implemented.
#endif // TUNDRA_PLATFORM_LINUX

#endif // TUNDRA_NOLIBC
