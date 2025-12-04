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


int main()
{
    if(Tundra_init() != 0)
    {
        fprintf(stderr, "Failed to initialize Tundra library!\n");
        return -1;
    }

    Tundra_DynamicArrayint arr;
    Tundra_DynArrint_init(&arr);

    Tundra_DynArrint_free(&arr);

    if(Tundra_shutdown() != 0)
    {
        fprintf(stderr, "Failed to shutdown Tundra library!\n");
        return -1;
    }

    return 0;
}