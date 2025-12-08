/**
 * @file SmallMemAlloc.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory sizes that are within 
 *    the defined size classes.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 */

#ifndef TUNDRA_SMALLMEMALLOC_H
#define TUNDRA_SMALLMEMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tundra/utils/CoreDef.h"

#define TUNDRA_MIN_SIZE_CLASS_MSB_POS 4
#define TUNDRA_MAX_SIZE_CLASS_MSB_POS 12
#define TUNDRA_NUM_SIZE_CLASSES (TUNDRA_EXPAND(TUNDRA_MAX_SIZE_CLASS_MSB_POS) -\
    TUNDRA_EXPAND(TUNDRA_MIN_SIZE_CLASS_MSB_POS) + 1)
#define TUNDRA_MAX_SIZE_CLASS_BYTE_SIZE (1ULL << \
    TUNDRA_EXPAND(TUNDRA_MAX_SIZE_CLASS_MSB_POS))

void InTundra_SmlMemAlc_init();

void InTundra_SmlMemAlc_shutdown();

bool InTundra_SmlMemAlc_is_ptr_in_arena(void *ptr);

void InTundra_SmlMemAlc_free(void *ptr);

void* InTundra_SmlMemAlc_malloc(uint64 num_bytes);

#ifdef __cplusplus
}
#endif

#endif
