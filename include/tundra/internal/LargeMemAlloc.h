/**
 * @file LargeMemAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory sizes that are greater 
 *    than the small allocator's defined size classes.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025 
*/

#ifndef TUNDRA_LARGEMEMALLOC_H
#define TUNDRA_LARGEMEMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tundra/utils/CoreDef.h"

void InTundra_LgMemAlc_init();

void InTundra_LgMemAlc_shutdown();

void InTundra_LgMemAlc_free(void *ptr);

void* InTundra_LgMemAlc_malloc(uint64 num_bytes);

#ifdef __cplusplus
}   
#endif

#endif
