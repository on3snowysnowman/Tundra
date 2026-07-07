/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"
#include "tundra/utils/ConsoleIO.h"
#include "tundra/common/ErrorDef.h"
#include "tundra/utils/StringConversion.h"

static void check_result(i64 result)
{
    if(result < 0)
    {
        Tundra_eprintf("Print failed with err: %s.\n", 
            Tundra_err_to_rdbl(result));
    }
}

#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    if (Tundra_init() != 0) return -1;

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
