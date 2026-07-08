/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"

#include "tundra/utils/FileHandling.h"
#include "tundra/utils/ConsoleIO.h"

int main(void)
{
    if (Tundra_init() != 0) return -1;

    Tundra_File file;
    
    Tundra_File_check_openerr(Tundra_File_open(&file, "Test.txt", 
        TUNDRA_FILE_OPEN_MODE_READWRITE, TUNDRA_FILE_OPEN_BEHAVIOR_TRUNCATE,
        true));

    Tundra_File_write_cstr(&file, "Hello World!\n");

    Tundra_readin_char(NULL);

    Tundra_File_check_closeerr(Tundra_File_close(&file));

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
