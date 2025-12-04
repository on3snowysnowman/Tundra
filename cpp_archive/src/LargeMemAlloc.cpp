/**
 * @file LargeMemAlloc.hpp
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
#include "tundra/internal/MemAllocHandler.hpp"
#include "tundra/utils/BitUtils.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/NumericLimits.hpp"


// Truncate namespace for local cpp file.
namespace MemAlias = Tundra::Internal::Mem::LargeAlloc;


// Definitions -----------------------------------------------------------------

// The maximum number of system memory pages per allocation that we will cache
// for reuse via malloc. Anything larger will be releasd back to the OS.
static constexpr Tundra::uint8 MAX_PAGE_SIZE_FOR_CACHING = 16;

// Maximum number of cached entries allowed for each page size.
static constexpr Tundra::uint8 MAX_CACHE_PER_PAGE_SIZE = 3;

static Tundra::uint64 max_allocation_byte_amount = 0;

// // Global limit on the amount of cached page sizes we will hold.
// static constexpr Tundra::uint8 MAX_TOTAL_PAGE_CACHE = 128;


// Containers ------------------------------------------------------------------

struct alignas(Tundra::Internal::Mem::DEFAULT_ALIGNMENT) BlockHeader
{
    // Size of the block this header tracks in page size increments.
    Tundra::uint32 block_page_size;
};
static constexpr Tundra::uint64 BLOCK_HEADER_SIZE = sizeof(BlockHeader);
static_assert(BLOCK_HEADER_SIZE % 
    Tundra::Internal::Mem::DEFAULT_ALIGNMENT == 0, "Header must be aligned");

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
 * The head array contains the head node of each list, whereas the tail list 
 * stores the tail node.
 */
static FreedBlock* cached_blocks_head[MAX_PAGE_SIZE_FOR_CACHING];
static FreedBlock* cached_blocks_tail[MAX_PAGE_SIZE_FOR_CACHING];

/**
 * Tracks the number of cached freed blocks per page size. Each index represents
 * the count cached of the index into the `cached_blocks` array.
 */
static Tundra::uint8 num_cached_per_page_size[MAX_PAGE_SIZE_FOR_CACHING];

// The total number of pages we currently have cached, kept under the  
// `MAX_TOTAL_PAGE_CACHE` limit.
// static Tundra::uint8 total_cached_pages;


// Methods ---------------------------------------------------------------------

// -- Local Methods --

/**
 * @brief Returns the number of cached blocks for a given page size.
 * 
 * @param page_size Page size to find the number of cached.
 *
 * @return Tundra::uint8 Number of cached blocks for the page.
 */
Tundra::uint8 get_num_cached(Tundra::uint32 page_size)
{
    // Index 0 corresponds to a page size of 1.
    return num_cached_per_page_size[page_size - 1];
}

Tundra::uint32 calc_page_size(Tundra::uint64 num_bytes)
{
    // Tundra::uint64 page_size_bytes = 
    //     Tundra::Internal::Mem::SystemMemData::page_size_bytes;

    // return (num_bytes + 
    //     (Tundra::Internal::Mem::SystemMemData::page_size_bytes - 1)) >>
    //     Tundra::get_num_trail_zeros(page_size_bytes);

    const Tundra::uint64 ps =
        Tundra::Internal::Mem::SystemMemData::page_size_bytes;

    const unsigned s = Tundra::get_num_trail_zeros(ps); 

    const Tundra::uint64 mask = ps - 1;                

    return (num_bytes >> s) + ((num_bytes & mask) != 0);
}

FreedBlock* get_freed_head_node(Tundra::uint32 page_size) 
{
    return cached_blocks_head[page_size - 1];
}

FreedBlock* get_freed_tail_node(Tundra::uint32 page_size) 
{
    return cached_blocks_tail[page_size - 1];
}


BlockHeader* get_header(void *ptr)
{
    return reinterpret_cast<BlockHeader*>(
        reinterpret_cast<Tundra::uint8*>(ptr) - BLOCK_HEADER_SIZE);
}

/**
 * @brief Get's the first available block that is the size of `page_size`.
 *
 * Assumes the number of cached blocks for the given page size is non zero.
 *
 * @param page_size Page size to find cached block of.
 *
 * @return void* 
 */
void* get_available_block(Tundra::uint32 page_size)
{
    FreedBlock *free_block = get_freed_head_node(page_size);

    Tundra::uint32 page_index = page_size - 1;

    // If there is only a single Node in the list.
    if(get_num_cached(page_size) == 1)
    {
        cached_blocks_head[page_index] = nullptr;
        cached_blocks_tail[page_index] = nullptr;
        --num_cached_per_page_size[page_size - 1];
        return reinterpret_cast<void*>(free_block);
    }

    // Set the new head node of the list to be whatever the snatched block was
    // pointing to next.
    cached_blocks_head[page_index] = free_block->next;
    cached_blocks_head[page_index]->prev = nullptr;

    --num_cached_per_page_size[page_size - 1];

    return reinterpret_cast<void*>(free_block);
}

/**
 * @brief Pops the stalest block off the linked list of FreedBlocks associated
 * with `page_size`.
 *
 * Assumes that the number of cached blocks for the page size is greater than 0.
 *
 * @note The return pointer pointer to the start of the usable memory of the 
 *    block, which is just after the header of the block.
 * 
 * @param page_size Page size to pop block from.
 */
void pop_stale_block(Tundra::uint32 page_size)
{
    FreedBlock *tail_node = get_freed_tail_node(page_size);

    Tundra::uint32 page_index = page_size - 1;

    // The begin of the block of memory the tail Node sits at.
    void *begin_mem_of_freed_block = reinterpret_cast<void*>(
        reinterpret_cast<Tundra::uint8*>(tail_node) - BLOCK_HEADER_SIZE);

    // There is only 1 Node in the list.
    if(get_num_cached(page_size) == 1)
    {
        // Release the only Node back to the OS.
        Tundra::Internal::Mem::release_mem_to_os(begin_mem_of_freed_block, 
            page_size * Tundra::Internal::Mem::SystemMemData::page_size_bytes);

        // There are now now Nodes in the list, so no head or tail.
        cached_blocks_head[page_index] = 
            cached_blocks_tail[page_index] = nullptr;

        --num_cached_per_page_size[page_index];
        return;
    }

    // -- There is more than 1 Node in the list --

    // Update the Node before the tail Node to point to nullptr;
    tail_node->prev->next = nullptr;

    // Set the previous Node to be the tail Node.
    cached_blocks_tail[page_index] = tail_node->prev;

    // Release the popped Node back to the OS.
    Tundra::Internal::Mem::release_mem_to_os(begin_mem_of_freed_block, 
        page_size * Tundra::Internal::Mem::SystemMemData::page_size_bytes);

    --num_cached_per_page_size[page_index];
}

void* create_new_block(Tundra::uint32 page_size)
{
    // Tundra::uint64 alloc_bytes;

    // const Tundra::uint64 PAGE_SIZE_BYTES = 
    //     Tundra::Internal::Mem::SystemMemData::page_size_bytes;

    void *mem = Tundra::Internal::Mem::get_mem_from_os(page_size * 
        Tundra::Internal::Mem::SystemMemData::page_size_bytes);

    // Set the header at the beginning of the new memory.
    reinterpret_cast<BlockHeader*>(mem)->block_page_size = page_size;

    // Return a pointer to the usable memory after the memory the header takes
    // up.
    return reinterpret_cast<void*>(reinterpret_cast<Tundra::uint8*>(mem) + 
        BLOCK_HEADER_SIZE);
}


// -- Public Methods --

void MemAlias::init()
{
    for(int i = 0; i < MAX_PAGE_SIZE_FOR_CACHING; ++i)
    {
        cached_blocks_head[i] = nullptr;
        cached_blocks_tail[i] = nullptr;
        num_cached_per_page_size[i] = 0;
    }

    max_allocation_byte_amount = Tundra::NumericLimits<Tundra::uint32>::max *
        Tundra::Internal::Mem::SystemMemData::page_size_bytes;
}

void MemAlias::free(void *ptr) 
{
    BlockHeader *hdr = get_header(ptr);
    Tundra::uint32 num_pages = hdr->block_page_size;

    if(num_pages == 0)
    {
        TUNDRA_FATAL("Attempted to free a block that had a page size of 0.");
    }

    // The size of this Block is larger than what we allow caching for, so just
    // release it immediately.
    if(num_pages > MAX_PAGE_SIZE_FOR_CACHING)
    {
        Tundra::Internal::Mem::release_mem_to_os(reinterpret_cast<void*>(hdr),
            num_pages * Tundra::Internal::Mem::SystemMemData::page_size_bytes);
        return;
    }

    // If we've reached our limit for total cached blocks for this page size.
    if(get_num_cached(num_pages) >= MAX_CACHE_PER_PAGE_SIZE)
    {
        // Pop off the most stale cached block which is at the end of the list
        // of freed blocks of this memory page size.
        pop_stale_block(num_pages);
    }

    Tundra::uint32 page_index = num_pages - 1;

    // Interpret the memory in front of the header as a FreedBlock, and use that
    // to store the information of a Node in our linked list.
    FreedBlock *new_free_block = reinterpret_cast<FreedBlock*>(
        reinterpret_cast<Tundra::uint8*>(hdr) + BLOCK_HEADER_SIZE);

    FreedBlock *existing_head_node = get_freed_head_node(num_pages);

    // We're adding a node to the chain
    ++num_cached_per_page_size[page_index];

    // Have the new Node point to what is the head Node.
    new_free_block->next = existing_head_node;
    new_free_block->prev = nullptr;

    // Update the head Node to now be the new Node.
    cached_blocks_head[page_index] = new_free_block;

    if(existing_head_node != nullptr)
    {
        // Have the head Node point back to the new block.
        existing_head_node->prev = new_free_block;
        return;
    }

    // -- Head node was a nullptr, which means the tail node is also a nullptr
    // and should be set to the only node in the list, which is our new 
    // freed block. --

    cached_blocks_tail[page_index] = new_free_block;
}

void* MemAlias::malloc(Tundra::uint64 num_bytes) 
{
    if(num_bytes > max_allocation_byte_amount)
    {
        TUNDRA_FATAL("Allocation amount is too large, limit is: %u",
            max_allocation_byte_amount);
    }

    Tundra::uint32 page_size = calc_page_size(num_bytes + 
        BLOCK_HEADER_SIZE);

    if(page_size == 0)
    {
        TUNDRA_FATAL("Calculated page size was 0.");
    }

    // If the number of pages that make up this memory if more than what we can
    // cache, it's not possible to have a cached block for this size. If this 
    // first check failed, then we check if there aren't any cached blocks for
    // this page size. If either are true, simply create a new block without 
    // looking for an available one.
    if(page_size > MAX_PAGE_SIZE_FOR_CACHING || get_num_cached(page_size) == 0)
    {
        return create_new_block(page_size);
    }

    return get_available_block(page_size);
}
