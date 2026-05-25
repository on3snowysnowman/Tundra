// MemAllocatorProfiler.cpp

#include <iostream>
#include <random>
#include <vector>
#include <sstream>

#include "tundra/Tundra.h"
#include "tundra/internal/SmallMemAlloc.h"
#include "tundra/utils/MemAlloc.h"

int get_rand_int(int min, int max)
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

struct AllocData
{
    int alloc_id = 0;
    void * mem = nullptr;
    int num_bytes = 0;
};

std::vector<AllocData> alloc_rand_mem()
{
    std::vector<AllocData> allocated_mem;

    int num_alloc = get_rand_int(3, 7);

    for(int i = 0; i < num_alloc; ++i)
    {
        AllocData data;

        data.alloc_id = i;
        data.num_bytes = get_rand_int(1, 500);
        data.mem = Tundra_alloc_mem(data.num_bytes);

        allocated_mem.push_back(data);
        std::cout << "Alloc id: " << data.alloc_id << '\n';
        std::cout << "Allocated " << data.num_bytes << " bytes\n\n";
    }

    return allocated_mem;
}

std::vector<AllocData> dealloc_some_mem(std::vector<AllocData> &alloced_mem)
{
    std::vector<AllocData> erased_mem;

    int num_to_dealloc = get_rand_int(2, alloced_mem.size() - 1);

    for(int i = 0; i < num_to_dealloc; ++i)
    {
        int dealloc_idx = get_rand_int(0, alloced_mem.size() - 1);

        Tundra_free_mem(alloced_mem.at(dealloc_idx).mem);

        erased_mem.push_back(alloced_mem.at(dealloc_idx));

        std::cout << "Deallocating alloc id: " << 
            alloced_mem.at(dealloc_idx).alloc_id << "\n\n";

        alloced_mem.erase(alloced_mem.begin() + dealloc_idx);
    }

    return erased_mem;
}

void realloc_some_mem(std::vector<AllocData> &dealloced_mem)
{
    int num_to_realloc = get_rand_int(1, dealloced_mem.size() - 1);

    for(int i = 0; i < num_to_realloc; ++i)
    {
        int realloc_idx = get_rand_int(0, dealloced_mem.size() - 1);

        const AllocData &targ_data = dealloced_mem.at(realloc_idx); 

        std::cout << "Alloc id: " << targ_data.alloc_id << '\n';
        std::cout << "Reallocating " << targ_data.num_bytes << " bytes\n\n";

        Tundra_alloc_mem(targ_data.num_bytes);

        dealloced_mem.erase(dealloced_mem.begin() + realloc_idx);
    }
}

std::string get_mem_alloc_status()
{        
    const MemArena * arena = get_mem_arena_instance();
    
    float ratio = float(arena->used_bytes) / arena->total_size_bytes;
    int ratio_as_percent = ratio * 100; 

    std::stringstream main_buf;

    main_buf << " -- ARENA --\n\n";
    main_buf << "Bytes used: " << arena->used_bytes << " / " << 
        arena->total_size_bytes << " (" << ratio_as_percent << "%)\n\n"; 

    u64 inspected_byte = 0;

    std::string display_buf = "[";

    int id = 0;

    while(inspected_byte < arena->used_bytes)
    {
        display_buf += '|' + std::to_string(id) + '|';

        // Get the BlockHeader of the inspected byte.
        const BlockHeader * header = 
            (const BlockHeader*)(arena->base_ptr + inspected_byte);

        main_buf << "Alloc id: " << id++ << '\n';
        main_buf << "Block size: " << header->block_byte_size << "\n\n";

        const u64 total_used_size = BLOCK_HEADER_SIZE + header->block_byte_size;
    
        char display = '.';

        if(header->in_use)
        {
            display = '*';
        }

        for(int i = 0; i < total_used_size / TUNDRA_MIN_SIZE_CLASS_BYTE_SIZE; 
            ++i)
        {
            display_buf += display;
        }

        inspected_byte += total_used_size;
    }

    display_buf += "| -> ...]\n"; 

    // for(int i = 0; i < (arena->total_size_bytes - arena->used_bytes) / 
    //     TUNDRA_MIN_SIZE_CLASS_BYTE_SIZE; ++i)
    // {
    //     display_buf += '.';
    // }

    main_buf << "\n" << display_buf << '\n';

    return main_buf.str();
}

void pause()
{
    std::cout << "Press enter to continue...";
    std::cin.get();
}

void clear_screen()
{
    system("clear");
    // std::cout << "\033[2J\033[H" << std::endl;
}

void clear_input()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void print_invalid_sel()
{
    clear_screen();
    std::cout << "Invalid input\n";
    pause();
}

// template<typename T>
// T prompt_type(const std::string &msg)
// {
//     while(true)
//     {
//         clear_screen();
//         std::cout << msg;

//         T in;

//         std::cin >> in;

//         if(!std::cin)
//         {
//             clear_input();
//             print_invalid_sel();
//             continue;
//         }

//         return in;
//     }
// }

template<typename T>
T prompt_type(const std::string& msg)
{
    while(true)
    {
        clear_screen();
        std::cout << msg;

        std::string line;
        std::getline(std::cin, line);

        std::stringstream ss(line);

        T value;

        if(ss >> value)
            return value;

        print_invalid_sel();
    }
}

void generate_random_profile()
{
    std::vector<AllocData> allocated_mem = alloc_rand_mem();
    std::vector<AllocData> deallocated_mem = dealloc_some_mem(allocated_mem);
    realloc_some_mem(deallocated_mem);

    std::cout << get_mem_alloc_status();
}

void alloc_menu()
{
}

void dealloc_menu()
{

}

void realloc_menu()
{

}

void main_menu()
{
    const std::string message = get_mem_alloc_status() + '\n' + 
        "Select an option:\n\n1: Allocate\n"
            "2: Deallocate\n3: Reallocate\n4: Quit\n\n   >> ";

    std::vector<AllocData> allocations;

    enum SELECTION
    {
        ALLOC = 1,
        DEALLOC,
        REALLOC,
        QUIT
    };

    while(true)
    {
        int selected = prompt_type<int>(message);

        switch(selected)
        {
            case ALLOC:

                alloc_menu();
                break;

            case DEALLOC:

                dealloc_menu();
                break;

            case REALLOC:

                realloc_menu();
                break;

            case QUIT:

                return;

            default:

                print_invalid_sel();
                break;
        }
    }
}


int main()
{
    Tundra_init();

    int selected = 0;

    while(true)
    {
        selected = prompt_type<int>("Choose profiler type:\n\n1: "
            "Interactive\n2: Random\n\n   >> ");

        if(selected == 1 || selected == 2) break;
        
        print_invalid_sel();
    }

    clear_screen();

    if(selected == 2) generate_random_profile();
    else main_menu();

    Tundra_shutdown();

    return 0;
}

