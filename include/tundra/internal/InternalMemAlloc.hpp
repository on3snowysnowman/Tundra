/**
 * @file InternalMemoryAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Internal methods for using the os to obtain and manage heap memory.
 * @version 0.1
 * @date 2025-10-18
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once

#include "tundra/internal/SystemCheck.hpp"
#include "tundra/utils/CoreTypes.hpp"


namespace Tundra::Internal::Mem
{

void init_mem_arena();

void free(void *ptr);

void* malloc(Tundra::uint64 num_bytes);

}
