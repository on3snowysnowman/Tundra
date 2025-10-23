/**
 * @file SmallMemAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory sizes that are greater 
 *    than the small allocator's defined size classes.
 * @version 0.1
 * @date 2025-10-23
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once 

#include "tundra/utils/CoreTypes.hpp"


namespace Tundra::Internal::Mem::LargeAlloc
{

void init();

void free(void *ptr);

void* malloc(Tundra::uint64 num_bytes);

} // Tundra::Internal::Mem::LargeAlloc
