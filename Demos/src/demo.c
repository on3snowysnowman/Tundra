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

    InTundra_IOBuffer buff;

    InTundra_IOBuff_init(&buff, TUNDRA_IOHANDLE_STDOUT);

    InTundra_IOBuff_write_bytes(&buff, (const u8*)"Hello World!\n", 13);

    char c;
    InTundra_read_bytes(TUNDRA_IOHANDLE_STDIN, &c, 1);

    InTundra_IOBuff_flush(&buff);

    InTundra_IOBuff_free(&buff);

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
