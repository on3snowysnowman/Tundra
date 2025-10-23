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

#include "tundra/internal/LargeMemAlloc.hpp"
#include "tundra/internal/InternalMemAlloc.hpp"


// Truncate namespace for local cpp file.
namespace MemAlias = Tundra::Internal::Mem::LargeAlloc;


// Definitions -----------------------------------------------------------------


// Containers ------------------------------------------------------------------

struct FreedBlock
{
    FreedBlock *next;
    FreedBlock *prev;
};

struct MemArena
{
    Tundra::uint8 *base_ptr;
    Tundra::uint64 used_bytes;
    Tundra::uint64 total_size_bytes;
};

struct alignas(Tundra::Internal::Mem::DEFAULT_ALIGNMENT) BlockHeader
{
    Tundra::uint64 block_byte_size;
    bool in_use;
    MemArena *mem_arena_parent;
};


// Global Variables ------------------------------------------------------------

// Sorted linked list from smallest -> largest byte size representing freed 
// blocks from any MemArena.
FreedBlock *freed_list;


// Methods --------------------------------------------------------------------

// -- Local Methods --


// -- Public Methods --

void MemAlias::init()
{
    
}

void MemAlias::free(void *ptr)
{

}

void* MemAlias::malloc(Tundra::uint64 num_bytes)
{

}
