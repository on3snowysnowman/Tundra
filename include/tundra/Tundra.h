/**
 * @file Tundra.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Entry to the library, provides init method.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_H
#define TUNDRA_H

#include "tundra/internal/MemAllocHandler.h"

/**
 * @brief Initializes any components in the library that need initializing. Must
 * be called before the library is used.
 * 
 * @return int 0 if successful. Error code otherwise.
 */
static inline int Tundra_init()
{
    InTundra_Mem_init();
    return 0;
}

#endif