
#include "tundra/internal/InternalMemAlloc.hpp"
#include "tundra/utils/FatalHandler.hpp"
#include "tundra/utils/BitUtils.hpp"
#include "tundra/utils/Math.hpp"
#include <iostream>

#ifdef TUNDRA_PLATFORM_POSIX
#include <unistd.h>
#include <sys/mman.h>

#else // Windows
#include <windows.h>

#endif

#ifdef TUNDRA_USE_C_MALLOC
#include <cstdlib>
#endif


// Truncate namespace for local cpp file.
namespace Mem = Tundra::Internal::Mem;


// Definitions -----------------------------------------------------------------

static constexpr Tundra::uint64 MEBIBYTE = Tundra::square(1024ULL);
static constexpr Tundra::uint8 DEFAULT_ALIGNMENT = 16;
static_assert(DEFAULT_ALIGNMENT >= 16, "Default alignment must be at least "
    "16.");

static constexpr Tundra::uint8 MIN_SIZE_CLASS_MSB_POS = 4;
static constexpr Tundra::uint8 MAX_SIZE_CLASS_MSB_POS = 12;
static constexpr Tundra::uint8 NUM_SIZE_CLASSES = 
    MAX_SIZE_CLASS_MSB_POS - MIN_SIZE_CLASS_MSB_POS + 1;

static_assert(Tundra::pow2(MIN_SIZE_CLASS_MSB_POS) >= 16, "Minimum size class "
    "must be at least the default alignment.");


// Containers ------------------------------------------------------------------

struct FreedBlock
{
    FreedBlock *next;
};

static_assert(sizeof(FreedBlock) <= Tundra::pow2(MIN_SIZE_CLASS_MSB_POS),
    "Size of FreedBlock struct must be within the minimum size class, "
    "otherwise the payload of each block is not enough to hold a FreedBlock "
    "object.");

struct MemArena
{
    Tundra::uint8 *base_ptr; // Pointer to the arena's memory start.
    Tundra::uint64 used_bytes; // Number of bytes currently used of the arena.
    Tundra::uint64 total_size_bytes; // Total size in bytes the arena holds.

    // Array of linked lists holding freed blocks of each size class.
    FreedBlock *freed_bins[NUM_SIZE_CLASSES];
};

struct SystemMemData
{
    static Tundra::uint64 page_size_bytes;
};
Tundra::uint64 SystemMemData::page_size_bytes;


struct alignas(DEFAULT_ALIGNMENT) BlockHeader
{
    Tundra::uint64 block_byte_size; // Number of bytes in the block.

    // Index into the size class lookup array, which represents the size class.
    Tundra::uint8 size_class_index; 

    bool in_use; // If this block is currently in use by the user.
};

static constexpr Tundra::uint8 BLOCK_HEADER_SIZE = sizeof(BlockHeader);

struct SizeClassLookup
{
    Tundra::uint16 data[NUM_SIZE_CLASSES];
};


// Global Variables ------------------------------------------------------------

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
// size class, which is 2^MIN_SIZE_CLASS_MSB_POS.
static constexpr SizeClassLookup size_class_lookup = make_size_class_lookup();
static_assert(size_class_lookup.data[0] >= DEFAULT_ALIGNMENT, "Smallest size "
    "class must be at least the default alignment.");

static MemArena arena;


// Methods ---------------------------------------------------------------------

// -- Local Functions -- 

/**
 * @brief Returns the index into the size class lookup of the smallest size
 * class that can hold `num_bytes`.
 * 
 * @param num_bytes Number of bytes to find suitable size class.
 *
 * @return Tundra::uint8 Index into the size class lookup. 
 */
Tundra::uint8 get_size_class_index(Tundra::uint64 num_bytes)
{
    bool is_pow_two = (num_bytes & (num_bytes - 1)) == 0;
    
    // Get the position of the most significant bit.
    Tundra::uint8 msb = 63ULL - Tundra::get_num_lead_zeros(num_bytes);

    // If num_bytes is a power of 2, it matches a size class exactly.
    // Otherwise, find the next power of 2 greater than num_bytes.
    // The msb index gives the size class bit. Add 1 if not a power of 2.
    // To map msb to free_bins index, subtract MIN_CLASS_MSB (4).
    // For example, 16 bytes (2^4) yields msb=4, so index 0.
    // This ensures the correct index for the smallest aligned size class.
    Tundra::uint8 size_class_index = 
        (is_pow_two ? msb : msb + 1) - MIN_SIZE_CLASS_MSB_POS;

    // Clamp the lookup index to the bounds of the array.
    return (size_class_index < NUM_SIZE_CLASSES) ? 
        size_class_index : NUM_SIZE_CLASSES - 1;
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
    return reinterpret_cast<BlockHeader*>(
        reinterpret_cast<Tundra::uint8*>(ptr) - BLOCK_HEADER_SIZE);
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
void* create_block(Tundra::uint8 size_class_index)
{
    Tundra::uint64 size_class_bytes = 
        size_class_lookup.data[size_class_index];

    std::cout << "Creating new block of size class: " << 
        size_class_bytes << '\n';

    // If we don't have enough room left to allocate. 
    if(size_class_bytes + BLOCK_HEADER_SIZE > 
        arena.total_size_bytes - arena.used_bytes)
    {
        TUNDRA_FATAL("Arena has run out of memory.");
    }

    // Get the Header of the new carved block.
    BlockHeader *header = reinterpret_cast<BlockHeader*>(
        arena.base_ptr + arena.used_bytes);

    header->block_byte_size = size_class_bytes;
    header->size_class_index = size_class_index;
    header->in_use = true;

    // Update the arena to reflect the new bytes we carved. We include the size
    // of the payload and the header.
    arena.used_bytes += size_class_bytes + BLOCK_HEADER_SIZE;

    // Return the pointer to the payload of the new Block.
    return reinterpret_cast<void*>(
        reinterpret_cast<Tundra::uint8*>(header) + BLOCK_HEADER_SIZE);
}

/**
 * @brief Gets a pointer to a block payload, whether that be a reused freed 
 * block or a newly carved block.
 * 
 * @param size_class_index Index into the size class lookup that represents
 * the size class of the needed block.
 *
 * @return void* Pointer to the block payload.
 */
void* get_block(Tundra::uint8 size_class_index)
{
    std::cout << "Getting block of size class: " << 
        size_class_lookup.data[size_class_index] << '\n';

    // If there are no available blocks for this size class
    if(arena.freed_bins[size_class_index] == nullptr)
    {
        std::cout << "No blocks available, creating new\n";
        return create_block(size_class_index);
    }

    std::cout << "There are blocks available\n";

    // -- There is an available block for this size class. --

    // Get the first block in the list.
    void *available_block = 
        reinterpret_cast<void*>(arena.freed_bins[size_class_index]);

    // Update the head node of this list to point to the next element, since 
    // we're taking the head node.
    arena.freed_bins[size_class_index] = 
        arena.freed_bins[size_class_index]->next;

    // Update the Header of the grabbed block to flag it as in use.
    get_header_from_payload_ptr(available_block)->in_use = true;

    return available_block;
}


// -- Public Interface --

void Mem::init()
{
    static constexpr Tundra::uint64 DEFAULT_ARENA_SIZE = MEBIBYTE;
    void *mem_from_os = nullptr;

    #ifdef TUNDRA_PLATFORM_POSIX
    long page_size = sysconf(_SC_PAGE_SIZE);

    if(page_size == -1)
    {
        TUNDRA_FATAL("Failed to get page size.");
    }

    SystemMemData::page_size_bytes = (Tundra::uint64)page_size;

    mem_from_os = mmap(nullptr, DEFAULT_ARENA_SIZE, PROT_READ|PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(mem_from_os == MAP_FAILED)
    {
        TUNDRA_FATAL("mmap failed.");
    }

    #else // Windows
    #error Not implemented yet.
    #endif

    arena.base_ptr = reinterpret_cast<Tundra::uint8*>(mem_from_os);
    arena.used_bytes = 0;
    arena.total_size_bytes = DEFAULT_ARENA_SIZE;

    // Initialize freed block linked lists for each size class.
    for(int i = 0; i < NUM_SIZE_CLASSES; ++i)
    {
        arena.freed_bins[i] = nullptr;
    }
}

void Mem::free(void *ptr)
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::free(ptr);
    #endif

    if(ptr == nullptr) { return; }

    Tundra::uint8 *reint_ptr = reinterpret_cast<Tundra::uint8*>(ptr);

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
    FreedBlock *new_freed_block = reinterpret_cast<FreedBlock*>(ptr);

    // Get a pointer to the head node pointer of the list belonging to the size
    // class of the freed payload.
    FreedBlock **ptr_head_node = &arena.freed_bins[header->size_class_index];

    if(*ptr_head_node == nullptr)
    {
        new_freed_block->next = nullptr;
        *ptr_head_node = new_freed_block;
        return;
    }

    // Since there is already node(s) existing in the size class linked list,
    // place the new freed block at the head node and have it point to what was
    // the head node.

    new_freed_block->next = *ptr_head_node;
    *ptr_head_node = new_freed_block;
}

void* Mem::malloc(Tundra::uint64 num_bytes)
{
    #ifdef TUNDRA_USE_C_MALLOC
    return ::malloc(num_bytes);
    #endif

    if(num_bytes == 0) 
    {
        TUNDRA_FATAL("Requested allocation of 0 bytes.");
    }

    if(num_bytes > size_class_lookup.data[NUM_SIZE_CLASSES - 1])
    {
        // Temporary 
        TUNDRA_FATAL("Requested bytes are too large to be represented by a "
            "size class.");
    }

    return get_block(get_size_class_index(num_bytes));
}