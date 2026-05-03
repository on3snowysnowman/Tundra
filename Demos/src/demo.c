/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"
#include "tundra/containers/DynamicStackint.h"

int main(void)
{
    if (Tundra_init() != 0)
    {
        return 2;
    }

    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init(&stk);

    int val = 0;

    Tundra_DynStkint_push_by_copy(&stk, &val);
    ++val;
    Tundra_DynStkint_push_by_move(&stk, &val);
    ++val;
    Tundra_DynStkint_push_by_val(&stk, val);
    *Tundra_DynStkint_push_uninit(&stk) = 3;

    const u64 size = Tundra_DynStkint_size(&stk);

    for(u64 i = 0; i < size; ++i)
    {
        printf("%llu: %d\n", i, *Tundra_DynStkint_front(&stk));
        Tundra_DynStkint_pop(&stk);
    }

    Tundra_DynStkint_free(&stk);

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
