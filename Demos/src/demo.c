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
#include "tundra/common/Core.h"
#include "tundra/common/ErrorDef.h"

int main(void)
{
    if (Tundra_init() != 0) return -1;

    i64 result;

    char c = Tundra_readin_char(&result);

    TUNDRA_RT_ASSERT(result >= 0, "Failed to read char: %s\n",
        Tundra_err_to_rdbl(result));

    Tundra_printf("Read char: %c\n", c);

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
