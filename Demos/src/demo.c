/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"
#include "tundra/internal/IOBuffer.h"

int main(void)
{
    if (Tundra_init() != 0) return -1;

    // Tundra_print_char(Tundra_readin_char(NULL));

    Tundra_DynamicArrayChar arr;
    Tundra_DynArrChar_init(&arr);

    Tundra_DynArrChar_add_val(&arr, 'c');

    Tundra_DynArrChar_erase_multiple(&arr, 0, 1);

    Tundra_DynArrChar_free(&arr);

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
