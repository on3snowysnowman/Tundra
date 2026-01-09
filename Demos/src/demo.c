/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tundra/Tundra.h"
#include "tundra/containers/DynamicArrayint.h"
#include "tundra/utils/SystemInfo.h"

#ifdef TUNDRA_NOLIBC
static int demo_main(void)
#else
int main(void)
#endif
{
    if(Tundra_init() != 0)
    {
        return 2;
    }

    Tundra_DynamicArrayint arr;
    Tundra_DynArrint_init(&arr);

    if(Tundra_shutdown() != 0)
    {
        return 2;
    }

    return 0;
}

#ifdef TUNDRA_NOLIBC

#ifdef TUNDRA_PLATFORM_LINUX

#ifdef TUNDRA_SYS_x86_64

#define TUNDRA_AT_NULL 0
#define TUNDRA_AT_PAGESIZE 6

typedef struct 
{
    uintptr a_type;
    uintptr a_val;
} auxv_t;

static uintptr InTundra_get_system_pagesize(uintptr *sp)
{
    // uintptr argc = sp[0];
    uintptr *p = sp + 1;

   // Skip argv pointers until NULL, then skip the NULL
    while (*p) { ++p; }
    ++p;

    // Skip envp pointers until NULL, then skip the NULL
    while(*p) { ++p; }
    ++p;

    // p should now point at auxv
    auxv_t *aux = (auxv_t *)p;

    for(; aux->a_type != TUNDRA_AT_NULL; ++aux)
    {
        if(aux->a_type == TUNDRA_AT_PAGESIZE)
        {
            return aux->a_val;
        }
    }

    // Return 0 on failure to find page size.
    return 0;
}

#undef TUNDRA_AT_NULL
#undef TUNDRA_AT_PAGESIZE

__attribute__((used))
static int InTundra_program_start(uintptr *sp)
{
    InTundra_Mem_data_instance.page_size_bytes = 
        InTundra_get_system_pagesize(sp);
    
    if (InTundra_Mem_data_instance.page_size_bytes == 0) { return 1; }
    return demo_main();
}

// Define start as naked so the compiler does not put anything before the stack
// pointer. 
__attribute__((naked, noreturn))
void _start(void); // Silence missing prototype warning
void _start()
{
    __asm__ volatile (
        "mov %rsp, %rdi\n"   // first arg: sp
        "call InTundra_program_start\n"
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
