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
#include "tundra/containers/DynamicStackint.h"

#include <stdio.h>

int main()
{
    if(Tundra_init() != 0)
    {
        fprintf(stderr, "Failed to initialize Tundra library!\n");
        return -1;
    } 

    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init(&stk);

    for(int i = 9; i >= 0; --i)
    {
        Tundra_DynStkint_push(&stk, &i);
    }

    printf("Size: %llu\n", Tundra_DynStkint_size(&stk));

    while(!Tundra_DynStkint_is_empty(&stk))
    {
        printf("%d\n", *Tundra_DynStkint_front(&stk));
        Tundra_DynStkint_pop(&stk);
    }

    printf("Size: %llu\n", Tundra_DynStkint_size(&stk));

    Tundra_DynStkint_free(&stk);

    if(Tundra_shutdown() != 0)
    {
        fprintf(stderr, "Failed to shutdown Tundra library!\n");
        return -1;
    }

    return 0;
}