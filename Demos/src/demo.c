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

#define TUNDRA_TYPE int
#define TUNDRA_CAP 8
#include "tundra/containers/Array.h"
#undef TUNDRA_TYPE
#undef TUNDRA_CAP

#include <stdio.h>

#define SIZE 100

int main()
{
    // if(Tundra_init() != 0)
    // {
    //     fprintf(stderr, "Failed to initialize Tundra library!\n");
    //     return -1;
    // }

    Tundra_Array8int arr = TUNDRA_MAKE_ARR(Tundra_Array8int, 
        1, 2, 3, 4, 59, 6, 7, 8);

    for(uint64 i = 0; i < Tundra_Arr8int_size(&arr); ++i)
    {
        printf("%d ", arr.data[i]);
    }

    printf("\n");

    // if(Tundra_shutdown() != 0)
    // {
    //     fprintf(stderr, "Failed to shutdown Tundra library!\n");
    //     return -1;
    // }

    return 0;
}