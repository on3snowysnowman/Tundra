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
#include "tundra/internal/SystemCheck.hpp"
// #include "tundra/internal/SmallMemAlloc.hpp"
// #include "tundra/utils/Math.hpp"
// #include "tundra/utils/BitUtils.hpp"


// Truncate namespace for local cpp file.
namespace MemAlias = Tundra::Internal::Mem::LargeAlloc;


// Definitions -----------------------------------------------------------------

// The maximum number of system memory pages per allocation that we will cache
// for reuse via malloc. Anything larger will be releasd back to the OS.
static constexpr Tundra::uint8 MAX_CACHED_PAGES_PER_ALLOCATION = 16;

// Maximum number of cached entries allowed for each page size.
static constexpr Tundra::uint8 MAX_CACHE_PER_PAGE_SIZE = 3;

// Global limit on the amount of cached page sizes we will hold.
static constexpr Tundra::uint8 MAX_TOTAL_PAGE_CACHE = 128;


// Containers ------------------------------------------------------------------

struct alignas(Tundra::Internal::Mem::DEFAULT_ALIGNMENT) BlockHeader
{
    // Size of the block this header tracks in page size increments.
    Tundra::uint8 block_page_size;
};
static constexpr Tundra::uint8 BLOCK_HEADER_SIZE = sizeof(BlockHeader);

struct alignas(Tundra::Internal::Mem::DEFAULT_ALIGNMENT) FreedBlock 
{
    FreedBlock *next; // Next FreedBlock in the chain, nullptr if none.
    FreedBlock *prev; // Previous FreedBlock in the chain, nullptr if none.
};


// Global Variables ------------------------------------------------------------

/**
 * Array of linked lists that contain cached blocks that have been freed. Each 
 * index corresponds to a block with an increment of a page size. For instance,
 * index 0 are blocks of 1 page, index 1 are blocks with 2 pages, and so on.
 */
static FreedBlock* cached_blocks[MAX_CACHED_PAGES_PER_ALLOCATION];

/**
 * Tracks the number of cached freed blocks per page size. Each index represents
 * the count cached of the index into the `cached_blocks` array.
 */
static Tundra::uint8 num_cached_per_page_size[MAX_CACHED_PAGES_PER_ALLOCATION];

// The total number of pages we currently have cached, kept under the  
// `MAX_TOTAL_PAGE_CACHE` limit.
static Tundra::uint8 total_cached_pages;


// Methods ---------------------------------------------------------------------

// -- Local Methods --

BlockHeader* get_header(void *ptr)
{
    return reinterpret_cast<BlockHeader*>(
        reinterpret_cast<Tundra::uint8*>(ptr) - BLOCK_HEADER_SIZE);
}


// -- Public Methods --

void MemAlias::init()
{
    for(int i = 0; i < MAX_CACHED_PAGES_PER_ALLOCATION; ++i)
    {
        cached_blocks[i] = nullptr;
        num_cached_per_page_size[i] = 0;
    }

    total_cached_pages = 0;
}

void MemAlias::free(void *ptr) 
{
    BlockHeader *hdr = get_header(ptr);

    if(total_cached_pages + hdr->block_page_size > MAX_TOTAL_PAGE_CACHE)
    {

    }
}

void* MemAlias::malloc(Tundra::uint64 num_bytes) 
{
    return nullptr;
}

