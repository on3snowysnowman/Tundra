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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "tundra/utils/CoreDef.h"

// Alignment for memory alignment inside of fetched chunks from the os.
#define TUNDRA_MEM_ALIGNMENT (short)16

// Required alignment of byte sizes when allocing from the os. This value must 
// be a power of 2.
#define TUNDRA_OS_ALLOC_ALIGNMENT (uint32)4096

// typedef struct __attribute__((aligned(TUNDRA_MEM_ALIGNMENT))) 
//     InTundra_SystemMemData
// {
//     uint64 page_size_bytes;
// } InTundra_SystemMemData;

// extern InTundra_SystemMemData InTundra_Mem_data_instance;

void InTundra_Mem_init(void);

void InTundra_Mem_shutdown(void);

void InTundra_Mem_free(void *ptr);

void* InTundra_Mem_malloc(uint64 num_bytes);

void InTundra_Mem_release_mem_to_os(void *ptr, uint64 num_bytes);

void *InTundra_Mem_get_mem_from_os(uint64 num_bytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TUNDRA_MEMALLOCHANDLER_H
