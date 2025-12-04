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
#include "tundra/internal/MemAllocHandler.h"
#include "tundra/utils/BitUtils.h"

#include <stdio.h>

#define SIZE 128

int main()
{
    Tundra_init();

    uint8 *mem = (uint8*)InTundra_Mem_malloc(SIZE);

    printf("Allocated %d bytes of memory at %p\n", SIZE, mem);

    if (mem != NULL)
    {
        printf("Memory allocation successful!\n");
    }
    else
    {
        printf("Memory allocation failed!\n");
    }

    InTundra_Mem_free(mem);

    mem = (uint8*)InTundra_Mem_malloc(SIZE);

    InTundra_Mem_free(mem);


    return 0;
}