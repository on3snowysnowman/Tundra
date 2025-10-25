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
#include "tundra/internal/MemAllocHandler.hpp"


namespace Tundra::Internal::Mem::SmallAlloc
{

static constexpr Tundra::uint8 MIN_SIZE_CLASS_MSB_POS = 4;
static constexpr Tundra::uint8 MAX_SIZE_CLASS_MSB_POS = 11;
static constexpr Tundra::uint8 NUM_SIZE_CLASSES = 
    MAX_SIZE_CLASS_MSB_POS - MIN_SIZE_CLASS_MSB_POS + 1;

static_assert(Tundra::pow2(MIN_SIZE_CLASS_MSB_POS) >= 16, "Minimum size class "
    "must be at least the default alignment.");
static_assert(NUM_SIZE_CLASSES ==
              (MAX_SIZE_CLASS_MSB_POS - MIN_SIZE_CLASS_MSB_POS + 1),
              "Class count must equal MSB range");
    
static constexpr Tundra::uint64 MAX_SIZE_CLASS_BYTE_SIZE = 
    Tundra::pow2(MAX_SIZE_CLASS_MSB_POS);


struct SizeClassLookup
{
    Tundra::uint16 data[NUM_SIZE_CLASSES];
};

consteval SizeClassLookup make_size_class_lookup()
{
    SizeClassLookup lookup;

    for(int i = 0; i < NUM_SIZE_CLASSES; ++i)
    {
        lookup.data[i] = Tundra::pow2(MIN_SIZE_CLASS_MSB_POS + i);
    }

    return lookup;
}

// Constant lookup for size classes. The 0th index corresponds to the smallest 
// size class, which is 2^MemAlias::MIN_SIZE_CLASS_MSB_POS.
static constexpr SizeClassLookup size_class_lookup = make_size_class_lookup();
static_assert(size_class_lookup.data[0] >= 
    Tundra::Internal::Mem::DEFAULT_ALIGNMENT, "Smallest size class must be at "
    "least the default alignment.");

/**
 * @brief Returns true if the given pointer exists within the memory arena's 
 * address.
 * 
 * @param ptr Pointer to check. 
 *
 * @return bool True if the address in the arena.
 */
bool is_ptr_in_arena(void *ptr);

void init();

void free(void *ptr);

void* malloc(Tundra::uint64 num_bytes);

} // namespace Tundra::Internal::Mem::SmallAlloc
