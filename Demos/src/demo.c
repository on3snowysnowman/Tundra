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
#include "tundra/containers/String.h"

#include <stdio.h>

void print_str(const Tundra_String *str)
{
    printf("String contents: %s\n", Tundra_Str_data(str));
    printf("String size: %llu\n", str->num_char);
    printf("String capacity: %llu\n", str->cap);
}


int main()
{
    if(Tundra_init() != 0)
    {
        fprintf(stderr, "Failed to initialize Tundra library!\n");
        return -1;
    }

    if(Tundra_shutdown() != 0)
    {
        fprintf(stderr, "Failed to shutdown Tundra library!\n");
        return -1;
    }

    return 0;
}