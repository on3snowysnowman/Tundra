/**
 * @file StdIO.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Basic methods for input and output of characters.
 * @version 0.1
 * @date 2025-11-11
 * 
 * @copyright Copyright (c) 2025
*/

#include "tundra/utils/StdIO.hpp"


// Definitions -----------------------------------------------------------------

static constexpr Tundra::uint64 DEF_BUF_SIZE = 16 * Tundra::KIBIBYTE;


// Global Variables ------------------------------------------------------------

static char buffer[DEF_BUF_SIZE];
static Tundra::uint64 total_buffered = 0;


// Internal --------------------------------------------------------------------

void flush()
{

}

void write_directly(const char *buf, Tundra::uint64 num_char)
{

}


// Public ----------------------------------------------------------------------

void Tundra::IO::write(const char *buf, Tundra::uint64 num_char)
{
    // The buffer is too small to hold this write size, simply write directly.
    if(num_char >= DEF_BUF_SIZE)
    {
        write_directly(buf, num_char);
        return;
    }
    
    // Buffer will be filled. Flush the buffer.
    if(total_buffered + num_char >= DEF_BUF_SIZE)
    {
        flush();
    }


}