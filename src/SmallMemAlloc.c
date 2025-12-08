/**
 * @file SmallMemAlloc.hpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Methods for allocating/freeing memory that is smaller than 
 * or equal to 4096 Bytes.
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025 
 */

#include "tundra/internal/SmallMemAlloc.h"
#include "tundra/internal/MacroHelper.h"
#include "tundra/utils/CoreDef.h"
#include "tundra/utils/BitUtils.h"
#include "tundra/utils/FatalHandler.h"
#include "tundra/internal/MemAllocHandler.h"


// Definitions -----------------------------------------------------------------

typedef struct SizeClassLookup
{
    uint16 data[TUNDRA_NUM_SIZE_CLASSES];
} SizeClassLookup;

// Constant lookup for size classes. The 0th index corresponds to the smallest 
// size class, which is 2^TUNDRA_MIN_SIZE_CLASS_MSB_POS.
static SizeClassLookup size_class_l_instance;

typedef struct FreedBlock
{
    struct FreedBlock *next;
} FreedBlock;

typedef struct TUNDRA_ALIGN(TUNDRA_MEM_DEF_ALIGN) BlockHeader
{
    uint64 block_byte_size; // Number of bytes in the block.

    // Index into the size class lookup array, which represents the size class.
    uint8 size_class_index; 

    bool in_use; // If this block is currently in use by the user.
} BlockHeader;

#define BLOCK_HEADER_SIZE sizeof(BlockHeader)

typedef struct MemArena
{
    uint8 *base_ptr; // Pointer to the arena's allocated memory start.
    uint64 used_bytes; // Number of bytes currently used of the arena.
    uint64 total_size_bytes; // Total size in bytes the arena holds.

    // Array of linked lists holding freed blocks of each size class.
    FreedBlock *freed_bins[TUNDRA_EXPAND(TUNDRA_NUM_SIZE_CLASSES)];
} MemArena;

// Global arena instance
static MemArena arena;


// Methods ---------------------------------------------------------------------

// -- Local Helper Methods --

void init_size_class_lookup()
{
    for(int i = 0; i < TUNDRA_EXPAND(TUNDRA_NUM_SIZE_CLASSES); ++i)
    {
        size_class_l_instance.data[i] = 
            (uint16)(1ULL << (TUNDRA_EXPAND(TUNDRA_MIN_SIZE_CLASS_MSB_POS) + i));
    }
}

/**
 * @brief Returns the index into the size class lookup of the smallest size
 * class that can hold `num_bytes`.
 * 
 * @param num_bytes Number of bytes to find suitable size class.
 *
 * @return uint8 Index into the size class lookup. 
 */
uint8 get_size_class_index(uint64 num_bytes)
{
    if(num_bytes <= size_class_l_instance.data[0])
    {
        return 0;
    }

    bool is_pow_two = (num_bytes & (num_bytes - 1)) == 0;

    // Get the position of the most significant bit.
    uint8 msb = 63ULL - Tundra_get_num_lead_zeros(num_bytes);

    // If num_bytes is a power of 2, it matches a size class exactly.
    // Otherwise, find the next power of 2 greater than num_bytes.
    // The msb index gives the size class bit. Add 1 if not a power of 2.
    // To map msb to free_bins index, subtract MIN_CLASS_MSB (4).
    // For example, 16 bytes (2^4) yields msb=4, so index 0.
    // This ensures the correct index for the smallest aligned size class.
    uint8 size_class_index = 
        (is_pow_two ? msb : msb + 1) - TUNDRA_MIN_SIZE_CLASS_MSB_POS;

    // Clamp the lookup index to the bounds of the array.
    return (size_class_index < TUNDRA_NUM_SIZE_CLASSES) ? 
        size_class_index : TUNDRA_NUM_SIZE_CLASSES - 1;
}

/**
 * @brief Given a pointer to a payload inside the arena, returns a pointer to
 * Header that tracks it.
 * 
 * @param ptr Pointer to the payload
 * @return BlockHeader* Pointer to the Header of the given payload.
 */
BlockHeader* get_header_from_payload_ptr(void * ptr)
{
    return (BlockHeader*)((uint8*)ptr - BLOCK_HEADER_SIZE);
}

/**
 * @brief Carves a new block from the arena and returns a pointer to its 
 * payload.
 * 
 * @param size_class_index Index into the size class lookup that represents
 * the size class of the block to create.
 *
 * @return void* Pointer to the new block payload. 
 */
void* create_block(uint8 size_class_index)
{
    const uint64 SIZE_CLASS_BYTES = 
        size_class_l_instance.data[size_class_index];

    // If we don't have enough room left to allocate. 
    if(SIZE_CLASS_BYTES + BLOCK_HEADER_SIZE > 
        arena.total_size_bytes - arena.used_bytes)
    {
        TUNDRA_FATAL("Out of memory in arena when trying "
            "to allocate %llu bytes.", SIZE_CLASS_BYTES);
    }

    // Get the Header of the new carved block
    BlockHeader *header = (BlockHeader*)(
        arena.base_ptr + arena.used_bytes);

    header->block_byte_size = SIZE_CLASS_BYTES;
    header->size_class_index = size_class_index;
    header->in_use = true;

    // Update the arena to reflect the new bytes we carved. We include the size
    // of the payload and the header.
    arena.used_bytes += SIZE_CLASS_BYTES + BLOCK_HEADER_SIZE;

    // Return the pointer to the payload of the new Block.
    return (void*)((uint8*)header + BLOCK_HEADER_SIZE);
}


// -- Public Methods --

#include <stdio.h>
void InTundra_SmlMemAlc_init() 
{
    init_size_class_lookup();

    static const uint64 DEF_ARENA_SIZE_BYTE = TUNDRA_MEBIBYTE;

    if(DEF_ARENA_SIZE_BYTE % InTundra_Mem_data_instance.page_size_bytes != 0)
    {
        TUNDRA_FATAL("Default arena size must be an increment of the system's "
            "page size.");
    }

    void *mem_from_os = InTundra_Mem_get_mem_from_os(DEF_ARENA_SIZE_BYTE);

    arena.base_ptr = (uint8*)mem_from_os;
    arena.used_bytes = 0;
    arena.total_size_bytes = DEF_ARENA_SIZE_BYTE;;

    // Initialize freed block linked lists for each size class.
    for(int i = 0; i < TUNDRA_NUM_SIZE_CLASSES; ++i)
    {
        arena.freed_bins[i] = NULL;
    }
}

void InTundra_SmlMemAlc_shutdown()
{
    InTundra_Mem_release_mem_to_os((void*)arena.base_ptr, 
        arena.total_size_bytes);
    arena.base_ptr = NULL;
}

bool InTundra_SmlMemAlc_is_ptr_in_arena(void *ptr)
{
    uint8 *cast_ptr = (uint8*)ptr;

    return cast_ptr >= arena.base_ptr && 
        cast_ptr < (arena.base_ptr + arena.total_size_bytes);
}

void InTundra_SmlMemAlc_free(void *ptr) 
{
    uint8 *reint_ptr = (uint8*)ptr;

    // If the ptr is outside the memory arena, it wasn't allocated by Tundra.
    if(reint_ptr < arena.base_ptr || 
        reint_ptr >= arena.base_ptr + arena.total_size_bytes)
    {
        TUNDRA_FATAL("Attempted to free pointer that was not malloced by "
            "Tundra: %p", ptr);
    }

    BlockHeader *header = get_header_from_payload_ptr(ptr);
    
    if(!header->in_use)
    {
        TUNDRA_FATAL("Attempted to free already freed memory: %p", ptr); 
    }

    header->in_use = false;

    // Interpret the payload as a freed block.
    FreedBlock *new_freed_block = (FreedBlock*)ptr;

    // Get a pointer to the head node pointer of the list belonging to the size
    // class of the freed payload.
    FreedBlock **ptr_head_node = &arena.freed_bins[header->size_class_index];

    if(*ptr_head_node == NULL)
    {
        new_freed_block->next = NULL;
        *ptr_head_node = new_freed_block;
        return;
    }

    // Since there are already node(s) existing in the size class linked list,
    // place the new freed block at the head node and have it point to what was
    // the head node.

    new_freed_block->next = *ptr_head_node;
    *ptr_head_node = new_freed_block;
}

void* InTundra_SmlMemAlc_malloc(uint64 num_bytes) 
{
    const uint8 SIZE_CLASS_INDEX = get_size_class_index(num_bytes);

    // printf("Allocating %llu bytes in size class index %u (%u bytes)\n", 
    //     num_bytes, SIZE_CLASS_INDEX, 
    //     size_class_l_instance.data[SIZE_CLASS_INDEX]);

    // If there are no available blocks for this size class
    if(arena.freed_bins[SIZE_CLASS_INDEX] == NULL)
    {
        // printf("No available blocks, creating new block.\n");
        return create_block(SIZE_CLASS_INDEX);
    }

    // printf("Reusing available block.\n");

    // -- There is an available block for this size class. --

    // Get the first block in the list.
    void *available_block = (void*)arena.freed_bins[SIZE_CLASS_INDEX];

    // Update the head node of this list to point to the next element, since 
    // we're taking the head node.
    arena.freed_bins[SIZE_CLASS_INDEX] = 
        arena.freed_bins[SIZE_CLASS_INDEX]->next;

    // Update the Header of the grabbed block to flag it as in use.
    get_header_from_payload_ptr(available_block)->in_use = true;

    // printf("Available block reused: %p\n", available_block);

    return available_block;
}
