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

#if defined(TUNDRA_PLATFORM_WINDOWS)
#include <windows.h>

#elif defined(TUNDRA_PLATFORM_POSIX)
#include <unistd.h>
#include <sys/mman.h>   
#include <errno.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#endif

namespace Mem = Tundra::Internal::Mem;


static constexpr Tundra::uint64 MEBIBYTE = 1024 * 1024;
static constexpr Tundra::uint64 DEFAULT_ALIGNMENT = 16;


struct MemArena
{
    Tundra::uint8 *base_ptr; // Pointer to the arena start.
    Tundra::uint64 used_bytes; // Number of bytes currently used of the arena.
    Tundra::uint64 total_size_bytes; // Total size in bytes of the arena.
};

struct SystemMemData
{
    Tundra::uint64 page_size_bytes;
};

struct BlockHeader
{
    Tundra::uint64 block_size;
    bool in_use;
};

struct FreedBlock
{
    FreedBlock *next;
};

// Global memory arena block. Must be initialized.
static MemArena mem_arena;
static SystemMemData sys_mem_d; 

// Head node of the freed block list.
static FreedBlock *freed_head_node = nullptr; 


static constexpr Tundra::uint64 align_up(Tundra::uint64 n, Tundra::uint64 a) 
{
    return (n + (a - 1)) & ~(a - 1);
}

static Tundra::uint64 round_to_page_size(Tundra::uint64 bytes_to_round)
{
    return align_up(bytes_to_round, sys_mem_d.page_size_bytes);
}

constexpr Tundra::uint64 BLOCK_HEADER_ALIGNED_SIZE =
    align_up(sizeof(BlockHeader), DEFAULT_ALIGNMENT);

BlockHeader* get_header_ptr(void *user_ptr)
{
    return reinterpret_cast<BlockHeader*>(
        reinterpret_cast<Tundra::uint8*>(user_ptr) - BLOCK_HEADER_ALIGNED_SIZE);
}

void* create_new_block(Tundra::uint64 num_bytes)
{
    
}

void* get_available_block(Tundra::uint64 num_bytes)
{
    FreedBlock *parsed_freed_block = freed_head_node;

    // We didn't find an available block that suits our size. Create a new one.
    return create_new_block(num_bytes);
}

void Mem::init_mem_arena()
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

    mem_arena.base_ptr = (Tundra::uint8*)mmap_ret_mem;
    mem_arena.used_bytes = 0;
    mem_arena.total_size_bytes = DEFAULT_ARENA_SIZE;

    #endif
}

void Mem::free(void *ptr)
{
    if(ptr == nullptr) { return; }

    if(ptr < mem_arena.base_ptr ||
        ptr > mem_arena.base_ptr + mem_arena.total_size_bytes)
    {
        TUNDRA_FATAL("Attempted to free pointer that was not malloced by "
            "Tundra : %p", ptr);
    }

    BlockHeader *header = get_header_ptr(ptr);
    if(!header->in_use) { return; }

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
}

void* Mem::malloc(Tundra::uint64 num_bytes)
{
    if(num_bytes == 0) { return nullptr; }

    if(freed_head_node != nullptr)
    {
        
    }

    const Tundra::uint64 ALIGNED_BYTE_AMNT = 
        align_up(num_bytes, DEFAULT_ALIGNMENT);

    // If we don't have enough room left to allocate the requested bytes
    if(ALIGNED_BYTE_AMNT + BLOCK_HEADER_ALIGNED_SIZE > 
        mem_arena.total_size_bytes - mem_arena.used_bytes)
    {
        return nullptr;
    }

    BlockHeader *new_header = reinterpret_cast<BlockHeader*>(
            mem_arena.base_ptr + mem_arena.used_bytes);

    new_header->block_size = ALIGNED_BYTE_AMNT;
    new_header->in_use = true;

    // Tundra::uint8 *return_ptr = mem_arena.base_ptr + mem_arena.used_bytes;

    mem_arena.used_bytes += ALIGNED_BYTE_AMNT + BLOCK_HEADER_ALIGNED_SIZE;

    return reinterpret_cast<void*>(reinterpret_cast<Tundra::uint8*>(new_header) 
        + BLOCK_HEADER_ALIGNED_SIZE);
}