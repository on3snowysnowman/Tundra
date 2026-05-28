/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"
#include "tundra/internal/IOInterface.h"
#include "tundra/utils/FileHandling.h"
#include "tundra/common/Core.h"
#include "tundra/utils/MemAlloc.h"
#include "tundra/utils/ConsoleOutput.h"


int main(void)
{
    if (Tundra_init() != 0)
    {
        return -1;
    }

    const char * const FILE_PATH = "test.txt";

    Tundra_File file;

    for(int i = 0; i < 10000; ++i)
    {

    i64 result = Tundra_file_open(&file, FILE_PATH,
        TUNDRA_FILE_OPEN_MODE_READONLY, TUNDRA_FILE_OPEN_BEHAVIOR_APPEND, 
        false);

    if(result < 0) return -2;

    const u64 file_byte_size = Tundra_file_get_size(&file);

    char *buffer = Tundra_alloc_mem(file_byte_size + 1);

    result = InTundra_read_bytes(file.handle, (void*)buffer, 
        (i64)file_byte_size);

    if(result < 0) return -2;

    Tundra_file_close(&file);

    buffer[file_byte_size] = '\0';

    Tundra_print_cstr(buffer);

    Tundra_print_char('\n');

    // result = Tundra_file_open(&file, FILE_PATH, 
    // TUNDRA_FILE_OPEN_MODE_WRITEONLY, TUNDRA_FILE_OPEN_BEHAVIOR_APPEND, 
    // true);

    // TUNDRA_RT_ASSERT(result >= 0, "Failed to open file: \"%s\" with error "
    //     "code: %d\n", FILE_PATH, (int)result);

    // result = Tundra_file_writef(&file, "This is some text with a number: %d\n", 
    //     -1239);

    // TUNDRA_RT_ASSERT(result >= 0, "Failed to write bytes with error: %d\n", 
    //     (int)result);

    // result = Tundra_file_close(&file);

    // TUNDRA_RT_ASSERT(result >= 0, "Failed to close file: \"%s\" with error "
    //     "code: %d\n", FILE_PATH, (int)result);

    }
    

    if (Tundra_shutdown() != 0)
    {
        return -1;
    }

    return 0;
}
