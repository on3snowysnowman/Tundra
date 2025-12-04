/**
 * @file MemAllocHandler.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for using the os to obtain and manage heap memory.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
*/

#ifndef TUNDRA_MEMALLOCHANDLER_H
#define TUNDRA_MEMALLOCHANDLER_H

#include "tundra/utils/CoreDef.h"

#define DEF_ALIGN 16

typedef struct __attribute__((aligned(DEF_ALIGN))) TundraIn_SystemMemData
{
    uint64 page_size_bytes;
} TundraIn_SystemMemData;

extern TundraIn_SystemMemData TundraIn_Mem_data_instance;

void TundraIn_Mem_init();

void TundraIn_Mem_free(void *ptr);

void* TundraIn_Mem_malloc(uint64 num_bytes);

void TundraIn_Mem_release_mem_to_os(void *ptr, uint64 num_bytes);

void *TundraIn_Mem_get_mem_from_os(uint64 num_bytes);

#undef DEF_ALIGNMENT

#endif