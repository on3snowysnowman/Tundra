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

#include "tundra/internal/InternalMemAlloc.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/BitUtils.hpp"
#include "tundra/utils/Math.hpp"


#if defined(TUNDRA_PLATFORM_WINDOWS)
#include <windows.h>

#elif defined(TUNDRA_PLATFORM_POSIX)
#include <unistd.h>
#include <sys/mman.h>   

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#endif

#ifdef TUNDRA_USE_C_MALLOC
#include <cstdlib> 
#endif

// Truncate namespace just for local cpp file.
namespace Mem = Tundra::Internal::Mem;


static constexpr Tundra::uint64 MEBIBYTE = 1024ULL * 1024ULL;
static constexpr Tundra::uint64 DEFAULT_ALIGNMENT = 16ULL;
static constexpr Tundra::uint8 NUM_SIZE_CLASSES = 8;

// Default alignment needs to be at least equal to 16, since our smallest size
// class is 16 and we assume in our internal functions that when bytes are 
// aligned, they are at least equal to 16 and therefore don't need to be 
// rounded up to a size class.
static_assert(DEFAULT_ALIGNMENT >= 16 && 
    (DEFAULT_ALIGNMENT & (DEFAULT_ALIGNMENT - 1)) == 0, "Default alignment "
    "must be a power of 2 and greater than or equal to 16");

// Containers ------------------------------------------------------------------

/**
 * @brief A chunk of memory allocated by the os that will be carved into smaller
 * pieces.
 * 
 */
struct MemArena
{
    Tundra::uint8 *base_ptr; // Pointer to the arena start.
    Tundra::uint64 used_bytes; // Number of bytes currently used of the arena.
    Tundra::uint64 total_size_bytes; // Total size in bytes of the arena.
};

/**
 * @brief Stores generic data for the System's Memory.
 * 
 */
struct SystemMemData
{
    Tundra::uint64 page_size_bytes;
};

/**
 * @brief Contains data for a block of memory partitioned from the arena. 
 *
 * A BlockHeader is placed before each chunk of memory carved from the arena.
 * `block_size` represents the number of bytes that belong to the block this 
 * Header tracks. `in_use` represents whether this block is currently in use by
 * the user. If it is false, this block is sitting inside the freed block list.
 * 
 */
struct alignas(16) BlockHeader
{
    Tundra::uint64 block_size;
    Tundra::uint16 class_size; // What class size this Header represents.
    bool in_use; // If the block this Header tracks is in use by the user.
};

/**
 * @brief Represents a block of memory that has been freed by the user and can
 * be reused.
 * 
 */
struct FreedBlock
{
    FreedBlock *next;
};


// Global Variables ------------------------------------------------------------

// Global memory arena block. Must be initialized.
static MemArena mem_arena;
// Global system memory data. Must be initialized.
static SystemMemData sys_mem_d; 

// Head node of the freed block list.
static FreedBlock *freed_head_node = nullptr; 

/**
 * Array of pointers to linked lists that contain empty blocks for each 
 * "size class". Index 0 = 16, Index 1 = 32... 
 */
static FreedBlock *free_bins[NUM_SIZE_CLASSES];


// Methods ---------------------------------------------------------------------

/**
 * @brief Aligns `bytes` up to `align`.
 * 
 * @param bytes Given bytes to align.
 * @param align Align to round up to.
 * @return Tundra::uint64 `bytes` aligned up to `align` 
 */
static constexpr Tundra::uint64 align_up(Tundra::uint64 bytes, 
    Tundra::uint64 align) 
{
    return (bytes + (align - 1)) & ~(align - 1);
}

static Tundra::uint64 round_to_page_size(Tundra::uint64 bytes_to_round)
{
    return align_up(bytes_to_round, sys_mem_d.page_size_bytes);
}

/**
 * @brief Returns the index into `free_bins` of the smallest size class for 
 * num_bytes.
 *
 * @param num_bytes Number of bytes to fit in a size class.
 * @return Tundra::uint8 Index of the minimum size class that fits num_bytes.
 */
Tundra::uint8 get_size_class_index(Tundra::uint64 num_bytes)
{
    num_bytes = Tundra::clamp_min(num_bytes, DEFAULT_ALIGNMENT);

    bool is_pow_two = (num_bytes & (num_bytes - 1)) == 0; 

    // Get the index of the most significant bit.
    Tundra::uint8 msb = 63U - Tundra::get_num_lead_zeros(num_bytes);

    // The minimum msb value for the smallest size class. 
    // The smallest size class is 16 bytes (2^4), so the msb for 16 is 4.
    // This constant is used to map the msb to the correct free_bins index.
    static constexpr Tundra::uint8 MIN_CLASS_MSB = 4;

    // If num_bytes is a power of 2, it matches a size class exactly.
    // Otherwise, find the next power of 2 greater than num_bytes.
    // The msb index gives the size class bit. Add 1 if not a power of 2.
    // To map msb to free_bins index, subtract MIN_CLASS_MSB (4).
    // For example, 16 bytes (2^4) yields msb=4, so index 0.
    // This ensures the correct index for the smallest aligned size class.
    Tundra::uint8 class_arr_index = 
        (is_pow_two ? msb : msb + 1) - MIN_CLASS_MSB;

    // Clamp the array index to the bounds of the array. If the 
    return (class_arr_index < NUM_SIZE_CLASSES) ? 
        class_arr_index : NUM_SIZE_CLASSES - 1;
}

constexpr Tundra::uint64 BLOCK_HEADER_ALIGNED_SIZE =
    align_up(sizeof(BlockHeader), DEFAULT_ALIGNMENT);

/**
 * @brief Returns a pointer to the BlockHeader tied to the memory chunk pointed 
 * to by `user_ptr`.
 * 
 * @param user_ptr Memory chunk to get BlockHeader of.
 * @return BlockHeader* BlockHeader of the given memory chunk.
 */
BlockHeader* get_header_ptr(void *user_ptr)
{
    return reinterpret_cast<BlockHeader*>(
        reinterpret_cast<Tundra::uint8*>(user_ptr) - BLOCK_HEADER_ALIGNED_SIZE);
}

/**
 * @brief Carves a new block of memory from the arena and returns a pointer to 
 * the start of the usable block.
 * 
 * @param num_bytes Minimum number of bytes to allocate for, assumed to be 
 * aligned.
 * @return void* Pointer to the usable memory block.
 */
void* create_new_block(Tundra::uint64 num_bytes)
{
    num_bytes = Tundra::clamp_min(num_bytes, DEFAULT_ALIGNMENT);

    // If we don't have enough room left to allocate the requested bytes
    if(num_bytes + BLOCK_HEADER_ALIGNED_SIZE > 
        mem_arena.total_size_bytes - mem_arena.used_bytes)
    {
        return nullptr;
    }

    BlockHeader *new_header = reinterpret_cast<BlockHeader*>(
            mem_arena.base_ptr + mem_arena.used_bytes);

    new_header->block_size = num_bytes;
    new_header->in_use = true;

    // Tundra::uint8 *return_ptr = mem_arena.base_ptr + mem_arena.used_bytes;

    mem_arena.used_bytes += num_bytes + BLOCK_HEADER_ALIGNED_SIZE;

    return reinterpret_cast<void*>(reinterpret_cast<Tundra::uint8*>(new_header) 
        + BLOCK_HEADER_ALIGNED_SIZE);
}

/**
 * @brief Returns the first available memory block, whether that be one that 
 * has been freed or a newly created one if no recycled ones are available.
 * 
 * @param num_bytes Minimum number of bytes to find block of, assumed to be 
 * aligned.
 * @return void* 
 */
void* get_available_block(Tundra::uint64 num_bytes)
{
    num_bytes = Tundra::clamp_min(num_bytes, DEFAULT_ALIGNMENT);

    // The variable that stores the pointer to the parsed node. We start this 
    // at the freed_head_node so link now points to the variable pointing to the
    // head node.
    FreedBlock **link = &freed_head_node;

    // Start the beginning node at the head node.
    // FreedBlock *parsed_freed_block = *link;

    // Iterate through the chain of freed blocks.
    while(FreedBlock *node = *link)
    {
        // Get the header of the parsed block.
        BlockHeader *parsed_header = 
            get_header_ptr(reinterpret_cast<void*>(node));

        // If this block is too small.
        if(parsed_header->block_size < num_bytes)
        {
            // Get the next freed block in the chain.
            link = &node->next;
            // parsed_freed_block = parsed_freed_block->next;
            continue;
        }

        // -- This block is a matching size -- .

        // Remove this FreedBlock from the list.
        *link = node->next;

        // Update the header to flag in use.        
        parsed_header->in_use = true;
        
        return reinterpret_cast<void*>(node);
    }

    // We didn't find an available block that suits our size. Create a new one.
    return create_new_block(num_bytes);
}

void Mem::init()
{   
    #if defined(TUNDRA_PLATFORM_WINDOWS)
    // SYSTEM_INFO sys_info;
    // GetSystemInfo(&sys_info);
    // sys_mem_d.page_size_bytes = sys_info.dwPageSize;
    // mem_arena.base_ptr = (Tundra::uint8*)VirtualAlloc(NULL, DEFAULT_ARENA_SIZE, 
    //     MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    // mem_arena.used_bytes = 0;
    // mem_arena.total_size_bytes = DEFAULT_ARENA_SIZE;
    // Disregard windows right now.
    
    #elif defined(TUNDRA_PLATFORM_POSIX)
    
    long page_size = sysconf(_SC_PAGESIZE); //NOLINT
    if(page_size == -1)
    {
        TUNDRA_FATAL("Failed to get page size.");
    }

    sys_mem_d.page_size_bytes = (Tundra::uint64)page_size;

    const Tundra::uint64 DEFAULT_ARENA_SIZE = 
        round_to_page_size(MEBIBYTE);

    void *mmap_ret_mem = mmap(nullptr, DEFAULT_ARENA_SIZE, 
        PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(mmap_ret_mem == MAP_FAILED)
    {
        TUNDRA_FATAL("Mmap failed.");
    }

    freed_head_node = nullptr;
    mem_arena.base_ptr = (Tundra::uint8*)mmap_ret_mem;
    mem_arena.used_bytes = 0;
    mem_arena.total_size_bytes = DEFAULT_ARENA_SIZE;

    #endif
}

void Mem::free(void *ptr)
{
    #ifdef TUNDRA_USE_C_MALLOC

    ::free(ptr);

    #else

    if(ptr == nullptr) { return; }

    Tundra::uint8 *ptr_as_uint8_ptr = reinterpret_cast<Tundra::uint8*>(ptr);

    // If the ptr is outside of the memory arena address range.
    if(ptr_as_uint8_ptr < mem_arena.base_ptr ||
        ptr_as_uint8_ptr >= mem_arena.base_ptr + mem_arena.total_size_bytes)
    {
        TUNDRA_FATAL("Attempted to free pointer that was not malloced by "
            "Tundra: %p", ptr);
    }

    BlockHeader *header = get_header_ptr(ptr);
    if(!header->in_use) 
    {
        TUNDRA_FATAL("Attempted to free the same pointer twice: %p", ptr); 
        return; 
    }

    header->in_use = false;

    FreedBlock *freed_block = reinterpret_cast<FreedBlock*>(ptr);

    if(freed_head_node == nullptr)
    {
        freed_head_node = freed_block;
        freed_head_node->next = nullptr;
        return;
    }

    freed_block->next = freed_head_node;
    freed_head_node = freed_block;

    #endif
}

void* Mem::malloc(Tundra::uint64 num_bytes)
{
    #ifdef TUNDRA_USE_C_MALLOC

    return ::malloc(num_bytes);

    #else

    if(num_bytes == 0) { return nullptr; }
    
    const Tundra::uint64 ALIGNED_BYTE_AMNT = 
        align_up(num_bytes, DEFAULT_ALIGNMENT);

    if(freed_head_node != nullptr)
    {
        return get_available_block(ALIGNED_BYTE_AMNT);
    }

    return create_new_block(ALIGNED_BYTE_AMNT);

    #endif
}