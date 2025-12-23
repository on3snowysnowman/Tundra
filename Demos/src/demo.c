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
#include "tundra/utils/Math.h"
#include "tundra/utils/MemAlloc.h"

#include <stdio.h>

int main()
{
    if(Tundra_init() != 0)
    {
        fprintf(stderr, "Failed to initialize Tundra library!\n");
        return -1;
    } 

    printf("%llu\n", InTundra_calc_new_capacity_by_doubling(10, 4)); // Expect: 16
    printf("%llu\n", InTundra_calc_new_capacity_by_doubling(120, 48) / 12); // Expect: 16

    if(Tundra_shutdown() != 0)
    {
        fprintf(stderr, "Failed to shutdown Tundra library!\n");
        return -1;
    }

    return 0;
}