/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>

#include "tundra/Tundra.h"
#include "tundra/containers/LinkedListint.h"

static void print_list(const Tundra_LinkedListint *list)
{
    printf(" == LIST == \nNum Elem : %llu\nCapacity: %llu\nElems: ",
        Tundra_LnkLstint_size(list), list->cap);

    Tundra_LinkedListIteratorint it = Tundra_LnkLstint_begin(list);
    Tundra_LinkedListIteratorint end = Tundra_LnkLstint_end(list);
    int i = 0;

    while(!Tundra_LnkLstIterint_compare(&it, &end))
    {
        printf("Index: %llu\n", it.index);
        printf("Datum: %d\n", list->nodes[it.index].datum);
        printf("Next: %llu\n", list->nodes[it.index].next);
        printf("Prev: %llu\n\n", list->nodes[it.index].prev);
        // printf("%d, ", *Tundra_LnkLstIterint_deref_cst(&it));
        Tundra_LnkLstIterint_next(&it);
    
        if(++i == 6) { break; }
    }

    printf("\nFront: %d\nBack: %d\n\n", *Tundra_LnkLstint_front_cst(list),
        *Tundra_LnkLstint_back_cst(list));
}

int main(void)
{
    if (Tundra_init() != 0)
    {
        return 2;
    }

    Tundra_LinkedListint list;
    Tundra_LnkLstint_init(&list);

    for(int i = 0; i < 2; ++i)
    {
        Tundra_LnkLstint_insert_at_idx_by_copy(&list, 0, &i);
    }

    print_list(&list);

    Tundra_LnkLstint_free(&list);

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
