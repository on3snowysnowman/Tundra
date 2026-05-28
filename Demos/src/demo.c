/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"

int main(void)
{
    if (Tundra_init() != 0)
    {
        return -1;
    }

    if (Tundra_shutdown() != 0)
    {
        return -1;
    }

    return 0;
}
