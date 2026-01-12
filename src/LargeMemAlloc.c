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

// The maximum size measured in increments of TUNDRA_OS_ALLOC_ALIGNMENT allowed
// for caching. 
#define MAX_ALIGN_INCR_FOR_CACHING 16

// Maximum number of cached entries allowed for allocation size. 
#define MAX_CACHE_PER_PAGE_SIZE 3

// Maximum number of bytes that can be requested to be allocated. 
#define MAX_ALLOC_BYTE_SIZE ((TUNDRA_UINT64_MAX / TUNDRA_OS_ALLOC_ALIGNMENT) * \
    TUNDRA_OS_ALLOC_ALIGNMENT)

// Containers ------------------------------------------------------------------

typedef struct TUNDRA_ALIGN(TUNDRA_MEM_ALIGNMENT) BlockHeader
{
    // Size of the block this header tracks in alignment increments.
    uint32 block_align_incr;
} BlockHeader;

#define BLOCK_HEADER_SIZE sizeof(BlockHeader)

typedef struct TUNDRA_ALIGN(TUNDRA_MEM_ALIGNMENT) FreedBlock
{
    struct FreedBlock *next; // Next FreedBlock in the chain, NULL if none.
    struct FreedBlock *prev; // Previous FreedBlock in the chain, NULL if none.
} FreedBlock;

// Global Variables ------------------------------------------------------------

/**
 * Array of linked lists that contain cached blocks that have been freed. Each 
 * index corresponds to a block with an increment of the os alignment. For 
 * instance, index 0 are blocks of 1 alignment, index 1 are blocks with 
 * 2 alignment increments, and so on. The head array contains the head node of 
 * each list, whereas the tail list stores the tail node.
 */
static FreedBlock* cached_blocks_head[MAX_ALIGN_INCR_FOR_CACHING];
static FreedBlock* cached_blocks_tail[MAX_ALIGN_INCR_FOR_CACHING];

/**
 * Tracks the number of cached freed blocks per alignment increment. Each index 
 * represents the count cached of the index into the `cached_blocks` array.
 */
static uint8 num_cached_per_align_incr[MAX_ALIGN_INCR_FOR_CACHING];

// Methods ---------------------------------------------------------------------

// -- Local Helper Methods --

/**
 * @brief Returns the number of cached blocks for a given alignment increment.
 * 
 * @param align_incr Alignment increment to find the number of cached.
 *
 * @return Tundra::uint8 Number of cached blocks for the increment.
 */
static uint8 get_num_cached(uint32 align_incr)
{
    // Index 0 corresponds to an alignment increment of 1.
    return num_cached_per_align_incr[align_incr - 1];
}

// #define TRAIL_ZEROS_OF_OS_ALLC_ALGN __builtin_ctzll(TUNDRA_OS_ALLOC_ALIGNMENT)

/**
 * @brief Given a number of bytes, calculates the minimum increment of 
 * TUNDRA_OS_ALLOC_ALIGNMENT that can represent those bytes.
 * 
 * @param num_bytes Number of bytes to calculate for.
 * 
 * @return uint32 Minimum increment of the alignment that can represent the 
 * given bytes.
 */
static uint32 calc_min_align_incr(uint64 num_bytes)
{
    enum 
    { 
        MASK = TUNDRA_OS_ALLOC_ALIGNMENT - 1,
        TRAIL_ZEROS_OF_OS_ALC_ALGN = 
            __builtin_ctzll(TUNDRA_OS_ALLOC_ALIGNMENT) 
    };

    // This operation is essentially a glorified ceiling division of 
    // `num_bytes` by TUNDRA_OS_ALLOC_ALIGNMENT. 
    // 
    // The first component in this operation divides the number of bytes to 
    // allocate by TUNDRA_OS_ALLOC_ALIGNMENT by utilizing the number of trailing
    // zeros present in it to bit shift, since the os alloc alignment is a power
    // of two. The second component of this operation is a check if the first
    // component is not an exact increment of the alloc alignment, and if it is
    // not, it adds 1 to the return increment amount to grab the next largest 
    // value.
    return (uint32)(num_bytes >> TRAIL_ZEROS_OF_OS_ALC_ALGN) + 
        ((num_bytes & MASK) != 0);
}

// #undef TRAIL_ZEROS_OF_OS_ALLC_ALGN

/**
 * @brief Returns the head Node of the linked list corresponding to the passed
 * alignment increment size.
 * 
 * Does not check if there exists a head Node, simply returns the pointer, which
 * may be NULL.
 * 
 * @param align_incr Alignment increment to get the head Node of the list.
 * 
 * @return FreedBlock* Freed block  
 */
static FreedBlock* get_freed_head_node(uint32 align_incr) 
{
    // Subtract 1 from the increment since an increment of 1 points to the 0th
    // index.
    return cached_blocks_head[align_incr - 1];
}

/**
 * @brief Returns the tail Node of the linked list corresponding to the passed
 * alignment increment size.
 * 
 * Does not check if there exists a tail Node, simply returns the pointer, which
 * may be NULL.
 * 
 * @param align_incr Alignment increment to get the tail Node of the list.
 * 
 * @return FreedBlock* Freed block  
 */
static FreedBlock* get_freed_tail_node(uint32 align_incr) 
{
    // Subtract 1 from the increment since an increment of 1 points to the 0th
    // index.
    return cached_blocks_tail[align_incr - 1];
}

/**
 * @brief Given a pointer to the beginning of a user-usable memory block, 
 * returns a pointer to the BlockHeader, which lives right before the usable 
 * mem block.
 * 
 * @param ptr Pointer to beginning of user-usable memory.
 *  
 * @return BlockHeader* Pointer to the BlockHeader of the mem block. 
 */
static BlockHeader* get_header(void *ptr)
{
    return (BlockHeader*)((uint8*)ptr - BLOCK_HEADER_SIZE);
}

/**
 * @brief Get's the first available block that has the alignment increment of
 * `align_incr`.
 *
 * Assumes the number of cached blocks for the given page size is non zero.
 *
 * @param align_incr Alignment increment to find cached block of.
 *
 * @return void* Pointer to the available block.
 */
static void* get_available_block(uint32 align_incr)
{
    FreedBlock *free_block = get_freed_head_node(align_incr);

    uint32 ALIGN_INCR_IDX = align_incr - 1;

    // If there is only a single Node in the list.
    if(get_num_cached(align_incr) == 1)
    {
        cached_blocks_head[ALIGN_INCR_IDX] = NULL;
        cached_blocks_tail[ALIGN_INCR_IDX] = NULL;
        --num_cached_per_align_incr[align_incr - 1];
        return (void*)free_block;
    }

    // Set the new head node of the list to be whatever the snatched block was
    // pointing to next.
    cached_blocks_head[ALIGN_INCR_IDX] = free_block->next;
    cached_blocks_head[ALIGN_INCR_IDX]->prev = NULL;

    --num_cached_per_align_incr[align_incr - 1];

    return (void*)free_block;
};

/**
 * @brief Pops the stalest block off the linked list of FreedBlocks associated
 * with `align_incr`.
 *
 * Assumes that the number of cached blocks for the page size is greater than 0.
 *
 * @note The return pointer pointer to the start of the usable memory of the 
 *    block, which is just after the header of the block.
 * 
 * @param align_incr Alignment increment to pop block from.
 */
static void pop_stale_block(uint32 align_incr)
{
    FreedBlock *tail_node = get_freed_tail_node(align_incr);

    // Subtract one from the increment since we account for an increment of 1
    // being represented as the 0th index into the cached array.
    const uint32 ALIGN_INCR_IDX = align_incr - 1;

    // The begin of the block of memory the tail Node sits at.
    void *begin_mem_of_freed_block = 
        (void*)((uint8*)(tail_node) - BLOCK_HEADER_SIZE);

    // There is only 1 Node in the list.
    if(get_num_cached(align_incr) == 1)
    {
        // Release the only Node back to the OS.
        InTundra_Mem_release_mem_to_os(begin_mem_of_freed_block, 
            align_incr * TUNDRA_OS_ALLOC_ALIGNMENT);

        // There are now now Nodes in the list, so no head or tail.
        cached_blocks_head[ALIGN_INCR_IDX] = 
            cached_blocks_tail[ALIGN_INCR_IDX] = NULL;

        --num_cached_per_align_incr[ALIGN_INCR_IDX];
        return;
    }

    // -- There is more than 1 Node in the list --

    // Update the Node before the tail Node to point to NULL;
    tail_node->prev->next = NULL;

    // Set the previous Node to be the tail Node.
    cached_blocks_tail[align_incr] = tail_node->prev;

    // Release the popped Node back to the OS.
    InTundra_Mem_release_mem_to_os(begin_mem_of_freed_block, 
        align_incr * TUNDRA_OS_ALLOC_ALIGNMENT);

    --num_cached_per_align_incr[ALIGN_INCR_IDX];
}

/**
 * @brief Creates a new block, allocating memory from the os and setting the 
 * information in the header of the new block. Returns a pointer to the usable
 * memory.
 * 
 * Uses `align_incr` to calculate the byte size to allocate. 
 * 
 * @param align_incr Byte size in increments of the os alignment to allocate 
 * for.
 *  
 * @return void* Pointer to the usable memory. 
 */
static void* create_new_block(uint32 align_incr)
{
    void *mem = InTundra_Mem_get_mem_from_os(align_incr * 
        TUNDRA_OS_ALLOC_ALIGNMENT);

    // Set the header at the beginning of the new memory.
    ((BlockHeader*)mem)->block_align_incr = align_incr;

    // Return a pointer to the usable memory after the memory the header takes
    // up.
    return (void*)((uint8*)(mem) + BLOCK_HEADER_SIZE);
}


// Public Methods --------------------------------------------------------------

void InTundra_LgMemAlc_init(void)
{
    for(int i = 0; i < MAX_ALIGN_INCR_FOR_CACHING; ++i)
    {
        cached_blocks_head[i] = NULL;
        cached_blocks_tail[i] = NULL;
        num_cached_per_align_incr[i] = 0;
    }

    // max_allocation_byte_amount = TUNDRA_UINT32_MAX *
    //     InTundra_Mem_data_instance.page_size_bytes;
}

void InTundra_LgMemAlc_shutdown(void)
{
    // Iterate through each page size
    for(int i = 1; i < MAX_ALIGN_INCR_FOR_CACHING; ++i)
    {
        FreedBlock *current_node = get_freed_head_node((uint32)i);

        // Release all cached blocks for this page size.
        while(current_node != NULL)
        {
            // The start of the block's memory is just before the header.
            void *begin_mem_of_freed_block = 
                (void*)((uint8*)(current_node) - BLOCK_HEADER_SIZE);

            InTundra_Mem_release_mem_to_os(begin_mem_of_freed_block, 
                (uint64)i * TUNDRA_OS_ALLOC_ALIGNMENT);

            current_node = current_node->next;
        }

        cached_blocks_head[i] = NULL;
        cached_blocks_tail[i] = NULL;
        num_cached_per_align_incr[i] = 0;
    }
}

void InTundra_LgMemAlc_free(void *ptr) 
{
    BlockHeader *hdr = get_header(ptr);
    uint32 num_align_incr = hdr->block_align_incr;

    if(num_align_incr == 0)
    {
        TUNDRA_FATAL("Attempted to free a block that had an alignment \
            increment of 0.");
    }

    // The size of this Block is larger than what we allow caching for, so just
    // release it immediately.
    if(num_align_incr > MAX_ALIGN_INCR_FOR_CACHING)
    {
        InTundra_Mem_release_mem_to_os((void*)(hdr),
            num_align_incr * TUNDRA_OS_ALLOC_ALIGNMENT);
        return;
    }

    // If we've reached our limit for total cached blocks for this alignment 
    // increment.
    if(get_num_cached(num_align_incr) >= MAX_CACHE_PER_PAGE_SIZE)
    {
        // Pop off the most stale cached block which is at the end of the list
        // of freed blocks of this alignment increment.
        pop_stale_block(num_align_incr);
    }

    // uint32 page_index = num_align_incr - 1;

    // Subtract one from the increment since we account for an increment of 1
    // being represented as the 0th index into the cached array.
    const uint32 ALIGN_INCR_IDX = num_align_incr - 1;

    // Interpret the memory in front of the header as a FreedBlock, and use that
    // to store the information of a Node in our linked list.
    FreedBlock *new_free_block = (FreedBlock*)(
        (uint8*)(hdr) + BLOCK_HEADER_SIZE);

    FreedBlock *existing_head_node = get_freed_head_node(num_align_incr);

    // We're adding a node to the chain
    ++num_cached_per_align_incr[ALIGN_INCR_IDX];

    // Have the new Node point to what is the head Node.
    new_free_block->next = existing_head_node;
    new_free_block->prev = NULL;

    // Update the head Node to now be the new Node.
    cached_blocks_head[ALIGN_INCR_IDX] = new_free_block;

    if(existing_head_node != NULL)
    {
        // Have the head Node point back to the new block.
        existing_head_node->prev = new_free_block;
        return;
    }

    // -- Head node was a NULL, which means the tail node is also a NULL
    // and should be set to the only node in the list, which is our new 
    // freed block. --

    cached_blocks_tail[ALIGN_INCR_IDX] = new_free_block;
}

void* InTundra_LgMemAlc_malloc(uint64 num_bytes) 
{
    if(num_bytes > MAX_ALLOC_BYTE_SIZE)
    {
        TUNDRA_FATAL("Allocation amount is too large, limit is: %u",
            MAX_ALLOC_BYTE_SIZE);
    }

    uint32 align_incr = calc_min_align_incr(num_bytes + 
        BLOCK_HEADER_SIZE);

    // If the number of pages that make up this memory if more than what we can
    // cache, it's not possible to have a cached block for this size. If this 
    // first check failed, then we check if there aren't any cached blocks for
    // this page size. If either are true, simply create a new block without 
    // looking for an available one.
    if(align_incr > MAX_ALIGN_INCR_FOR_CACHING || 
        get_num_cached(align_incr) == 0)
    {
        return create_new_block(align_incr);
    }

    return get_available_block(align_incr);
}
