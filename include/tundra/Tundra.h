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
#include "tundra/utils/ConsoleIO.h"
#include "tundra/utils/StringConversion.h"
#include "tundra/utils/Math.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes any components in the library that need initializing. Must
 * be called before the library is used.
 * 
 * @return int 0 if successful. Error code otherwise.
 */
static inline int Tundra_init()
{
    InTundra_Mem_init();
    InTundra_ConIO_init();

    InTundra_float_rounding_constant = ((float)1 / (float)Tundra_uint_pow(10,
        TUNDRA_FLOAT_PRECISION + 1)) * 5;

    return 0;
}

/**
 * @brief Shuts down any components in the library that need shutting down.
 * Must be called when the library is no longer needed.
 * 
 */
static inline int Tundra_shutdown()
{
    InTundra_ConIO_shutdown();
    InTundra_Mem_shutdown();
    return 0;
}

#ifdef __cplusplus
}   
#endif

#endif