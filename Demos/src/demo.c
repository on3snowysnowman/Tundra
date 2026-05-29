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


int main(void)
{
    if (Tundra_init() != 0) return -1;

    const char * const FILE_PATH = "Test.txt";

    Tundra_File file;

    i64 result = Tundra_file_open(&file, FILE_PATH, 
        TUNDRA_FILE_OPEN_MODE_WRITEONLY, TUNDRA_FILE_OPEN_BEHAVIOR_TRUNCATE, 
        true);

    Tundra_file_check_openerr(result);
    
    result = Tundra_file_writef(&file, "This is a formatted message: %d\n", 
        123);

    Tundra_file_check_writeerr(result);

    result = Tundra_file_close(&file);

    Tundra_file_check_closeerr(result);

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
