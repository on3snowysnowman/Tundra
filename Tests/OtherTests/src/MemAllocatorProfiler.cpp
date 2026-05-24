// MemAllocatorProfiler.cpp

#include <iostream>
#include <random>
#include <vector>

#include "tundra/Tundra.h"
#include "tundra/internal/SmallMemAlloc.h"
#include "tundra/utils/MemAlloc.h"

int get_rand_int(int min, int max)
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

std::vector<void*> alloc_rand_mem()
{
    std::vector<void*> allocated_mem;

    int num_alloc = get_rand_int(1, 7);

    for(int i = 0; i < num_alloc; ++i)
    {
        u64 alloc_amount = get_rand_int(1, 500);
        allocated_mem.push_back(Tundra_alloc_mem(alloc_amount));
        std::cout << "Allocated " << alloc_amount << " bytes\n";
    }

    std::cout << '\n';

    return allocated_mem;
}

void dealloc_some_mem(std::vector<void *> &alloced_mem)
{
    int num_to_dealloc = get_rand_int(1, alloced_mem.size() - 1);

    for(int i = 0; i < num_to_dealloc; ++i)
    {
        int dealloc_idx = get_rand_int(0, alloced_mem.size() - 1);

        Tundra_free_mem(alloced_mem.at(dealloc_idx));

        alloced_mem.erase(alloced_mem.begin() + dealloc_idx);
    }
}

void output_mem_alloc_status()
{        
    const MemArena * arena = get_mem_arena_instance();

    std::cout << "Profiling Arena of size: " << arena->total_size_bytes << 
        " bytes.\n";
    std::cout << "Total bytes used: " << arena->used_bytes << "\n\n";

    u64 inspected_byte = 0;

    std::string output_buf = "[";

    while(inspected_byte < arena->used_bytes)
    {
        // Get the BlockHeader of the inspected byte.
        const BlockHeader * header = 
            (const BlockHeader*)(arena->base_ptr + inspected_byte);

        std::cout << "Inspecting header @ byte: " << inspected_byte << '\n';
        std::cout << "In use: " << header->in_use << '\n';
        std::cout << "Block size: " << header->block_byte_size << '\n';

        const u64 total_used_size = BLOCK_HEADER_SIZE + header->block_byte_size;
    
        char display = '.';

        if(header->in_use)
        {
            display = '*';
        }

        for(int i = 0; i < total_used_size / TUNDRA_MIN_SIZE_CLASS_BYTE_SIZE; 
            ++i)
        {
            output_buf += display;
        }

        output_buf += '|';

        inspected_byte += total_used_size;

        std::cout << '\n';
    }

    for(int i = 0; i < (arena->total_size_bytes - arena->used_bytes) / 
        TUNDRA_MIN_SIZE_CLASS_BYTE_SIZE; ++i)
    {
        output_buf += '.';
    }

    output_buf += "]\n";

    std::cout << "\n" << output_buf << '\n';
}

int main()
{
    Tundra_init();

    std::vector<void*> allocated_mem = alloc_rand_mem();
    dealloc_some_mem(allocated_mem);

    output_mem_alloc_status();

    Tundra_shutdown();

    return 0;
}

