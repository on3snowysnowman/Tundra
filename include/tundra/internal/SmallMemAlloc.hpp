/**
 * @file SmallMemAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory sizes that are within 
 *    the defined size classes.
 * @version 0.1
 * @date 2025-10-23
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#pragma once

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/utils/Math.hpp"


namespace Tundra::Internal::Mem::SmallAlloc
{

static constexpr Tundra::uint8 MIN_SIZE_CLASS_MSB_POS = 4;
static constexpr Tundra::uint8 MAX_SIZE_CLASS_MSB_POS = 12;
static constexpr Tundra::uint8 NUM_SIZE_CLASSES = 
    MAX_SIZE_CLASS_MSB_POS - MIN_SIZE_CLASS_MSB_POS + 1;

static_assert(Tundra::pow2(MIN_SIZE_CLASS_MSB_POS) >= 16, "Minimum size class "
    "must be at least the default alignment.");
static_assert(NUM_SIZE_CLASSES ==
              (MAX_SIZE_CLASS_MSB_POS - MIN_SIZE_CLASS_MSB_POS + 1),
              "Class count must equal MSB range");
    
static constexpr Tundra::uint64 MAX_SIZE_CLASS_BYTE_SIZE = 
    Tundra::pow2(MAX_SIZE_CLASS_MSB_POS);

void init();

void free(void *ptr);

void* malloc(Tundra::uint64 num_bytes);

} // namespace Tundra::Internal::Mem::SmallAlloc
