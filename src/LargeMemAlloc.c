/**
 * @file LargeMemAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory sizes that are greater 
 *    than the small allocator's defined size classes.
 * @date 2025-12-04
 * 
 * @copyright Copyright (c) 2025
 */

#include "tundra/internal/LargeMemAlloc.h"
#include "tundra/internal/MemAllocHandler.h"
#include "tundra/utils/NumLimits.h"
#include "tundra/utils/BitUtils.h"
#include "tundra/utils/FatalHandler.h"

// Defintions -----------------------------------------------------------------

// The maximum number of system memory pages per allocation that we will cache
// for reuse via malloc. Anything larger will be releasd back to the OS.
#define MAX_PAGE_SIZE_FOR_CACHING 16

// Maximum number of cached entries allowed for each page size.
#define MAX_CACHE_PER_PAGE_SIZE 3

static uint64 max_allocation_byte_amount = 0;

// Containers ------------------------------------------------------------------

typedef struct TUNDRA_ALIGN(TUNDRA_MEM_DEF_ALIGN) BlockHeader
{
    // Size of the block this header tracks in page size increments.
    uint32 block_page_size;
} BlockHeader;

#define BLOCK_HEADER_SIZE sizeof(BlockHeader)

typedef struct TUNDRA_ALIGN(TUNDRA_MEM_DEF_ALIGN) FreedBlock
{
    struct FreedBlock *next; // Next FreedBlock in the chain, NULL if none.
    struct FreedBlock *prev; // Previous FreedBlock in the chain, NULL if none.
} FreedBlock;

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
static uint8 num_cached_per_page_size[MAX_PAGE_SIZE_FOR_CACHING];

// Methods ---------------------------------------------------------------------

// -- Local Helper Methods --

/**
 * @brief Returns the number of cached blocks for a given page size.
 * 
 * @param page_size Page size to find the number of cached.
 *
 * @return Tundra::uint8 Number of cached blocks for the page.
 */
uint8 get_num_cached(uint32 page_size)
{
    // Index 0 corresponds to a page size of 1.
    return num_cached_per_page_size[page_size - 1];
}

uint32 calc_page_size(uint64 num_bytes)
{
    const uint64 ps = InTundra_Mem_data_instance.page_size_bytes;

    const unsigned s = Tundra_get_num_trail_zeros(ps); 

    const uint64 mask = ps - 1;                

    return (num_bytes >> s) + ((num_bytes & mask) != 0);
}

FreedBlock* get_freed_head_node(uint32 page_size) 
{
    return cached_blocks_head[page_size - 1];
}

FreedBlock* get_freed_tail_node(uint32 page_size) 
{
    return cached_blocks_tail[page_size - 1];
}


BlockHeader* get_header(void *ptr)
{
    return (BlockHeader*)((uint8*)ptr - BLOCK_HEADER_SIZE);
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
void* get_available_block(uint32 page_size)
{
    FreedBlock *free_block = get_freed_head_node(page_size);

    uint32 page_index = page_size - 1;

    // If there is only a single Node in the list.
    if(get_num_cached(page_size) == 1)
    {
        cached_blocks_head[page_index] = NULL;
        cached_blocks_tail[page_index] = NULL;
        --num_cached_per_page_size[page_size - 1];
        return (void*)free_block;
    }

    // Set the new head node of the list to be whatever the snatched block was
    // pointing to next.
    cached_blocks_head[page_index] = free_block->next;
    cached_blocks_head[page_index]->prev = NULL;

    --num_cached_per_page_size[page_size - 1];

    return (void*)free_block;
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
void pop_stale_block(uint32 page_size)
{
    FreedBlock *tail_node = get_freed_tail_node(page_size);

    uint32 page_index = page_size - 1;

    // The begin of the block of memory the tail Node sits at.
    void *begin_mem_of_freed_block = 
        (void*)((uint8*)(tail_node) - BLOCK_HEADER_SIZE);

    // There is only 1 Node in the list.
    if(get_num_cached(page_size) == 1)
    {
        // Release the only Node back to the OS.
        InTundra_Mem_release_mem_to_os(begin_mem_of_freed_block, 
            page_size * InTundra_Mem_data_instance.page_size_bytes);

        // There are now now Nodes in the list, so no head or tail.
        cached_blocks_head[page_index] = 
            cached_blocks_tail[page_index] = NULL;

        --num_cached_per_page_size[page_index];
        return;
    }

    // -- There is more than 1 Node in the list --

    // Update the Node before the tail Node to point to NULL;
    tail_node->prev->next = NULL;

    // Set the previous Node to be the tail Node.
    cached_blocks_tail[page_index] = tail_node->prev;

    // Release the popped Node back to the OS.
    InTundra_Mem_release_mem_to_os(begin_mem_of_freed_block, 
        page_size * InTundra_Mem_data_instance.page_size_bytes);

    --num_cached_per_page_size[page_index];
}

void* create_new_block(uint32 page_size)
{
    void *mem = InTundra_Mem_get_mem_from_os(page_size * 
        InTundra_Mem_data_instance.page_size_bytes);

    // Set the header at the beginning of the new memory.
    ((BlockHeader*)mem)->block_page_size = page_size;

    // Return a pointer to the usable memory after the memory the header takes
    // up.
    return (void*)((uint8*)(mem) + BLOCK_HEADER_SIZE);
}


// -- Public Methods --

void InTundra_LgMemAlc_init()
{
    for(int i = 0; i < MAX_PAGE_SIZE_FOR_CACHING; ++i)
    {
        cached_blocks_head[i] = NULL;
        cached_blocks_tail[i] = NULL;
        num_cached_per_page_size[i] = 0;
    }

    max_allocation_byte_amount = TUNDRA_UINT32_MAX *
        InTundra_Mem_data_instance.page_size_bytes;
}

void InTundra_LgMemAlc_shutdown()
{
    // Iterate through each page size
    for(int i = 1; i < MAX_PAGE_SIZE_FOR_CACHING; ++i)
    {
        FreedBlock *current_node = get_freed_head_node(i);

        // Release all cached blocks for this page size.
        while(current_node != NULL)
        {
            // The start of the block's memory is just before the header.
            void *begin_mem_of_freed_block = 
                (void*)((uint8*)(current_node) - BLOCK_HEADER_SIZE);

            InTundra_Mem_release_mem_to_os(begin_mem_of_freed_block, 
                i * InTundra_Mem_data_instance.page_size_bytes);

            current_node = current_node->next;
        }

        cached_blocks_head[i] = NULL;
        cached_blocks_tail[i] = NULL;
        num_cached_per_page_size[i] = 0;
    }
}

void InTundra_LgMemAlc_free(void *ptr) 
{
    BlockHeader *hdr = get_header(ptr);
    uint32 num_pages = hdr->block_page_size;

    if(num_pages == 0)
    {
        TUNDRA_FATAL("Attempted to free a block that had a page size of 0.");
    }

    // The size of this Block is larger than what we allow caching for, so just
    // release it immediately.
    if(num_pages > MAX_PAGE_SIZE_FOR_CACHING)
    {
        InTundra_Mem_release_mem_to_os((void*)(hdr),
            num_pages * InTundra_Mem_data_instance.page_size_bytes);
        return;
    }

    // If we've reached our limit for total cached blocks for this page size.
    if(get_num_cached(num_pages) >= MAX_CACHE_PER_PAGE_SIZE)
    {
        // Pop off the most stale cached block which is at the end of the list
        // of freed blocks of this memory page size.
        pop_stale_block(num_pages);
    }

    uint32 page_index = num_pages - 1;

    // Interpret the memory in front of the header as a FreedBlock, and use that
    // to store the information of a Node in our linked list.
    FreedBlock *new_free_block = (FreedBlock*)(
        (uint8*)(hdr) + BLOCK_HEADER_SIZE);

    FreedBlock *existing_head_node = get_freed_head_node(num_pages);

    // We're adding a node to the chain
    ++num_cached_per_page_size[page_index];

    // Have the new Node point to what is the head Node.
    new_free_block->next = existing_head_node;
    new_free_block->prev = NULL;

    // Update the head Node to now be the new Node.
    cached_blocks_head[page_index] = new_free_block;

    if(existing_head_node != NULL)
    {
        // Have the head Node point back to the new block.
        existing_head_node->prev = new_free_block;
        return;
    }

    // -- Head node was a NULL, which means the tail node is also a NULL
    // and should be set to the only node in the list, which is our new 
    // freed block. --

    cached_blocks_tail[page_index] = new_free_block;
}

void* InTundra_LgMemAlc_malloc(uint64 num_bytes) 
{
    if(num_bytes > max_allocation_byte_amount)
    {
        TUNDRA_FATAL("Allocation amount is too large, limit is: %u",
            max_allocation_byte_amount);
    }

    uint32 page_size = calc_page_size(num_bytes + 
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
