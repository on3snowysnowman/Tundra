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

#include <stdio.h>

void print_array(const Tundra_DynamicArrayint *arr)
{
    for(int i = 0; i < Tundra_DynArrint_size(arr); ++i)
    {
        printf("i = %d: %d\n", i, *Tundra_DynArrint_at_cst(arr, i));
    }

    puts("");
}

int main()
{
    if(Tundra_init() != 0)
    {
        fprintf(stderr, "Failed to initialize Tundra library!\n");
        return -1;
    } 

    Tundra_DynamicArrayint arr;
    Tundra_DynArrint_init(&arr);

    for(int i = 0; i < 15; ++i)
    {
        Tundra_DynArrint_add(&arr, &i);
    }

    print_array(&arr);

    *Tundra_DynArrint_at(&arr, 1) = 30;

    print_array(&arr);

    Tundra_DynArrint_shrink_to_new_cap(&arr, 9);

    print_array(&arr);
    

    if(Tundra_shutdown() != 0)
    {
        fprintf(stderr, "Failed to shutdown Tundra library!\n");
        return -1;
    }

    return 0;
}