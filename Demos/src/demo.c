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

    Tundra_print_fmt("Formatted text: %d\n", 324);

    Tundra_print_float(3.1235f);

    Tundra_flush_stdout();

    char buff[2];

    InTundra_raw_read_bytes(TUNDRA_IOHANDLE_STDIN, buff, 2);

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
