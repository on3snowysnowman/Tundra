
#include <iostream>
#include <fstream>
// #include <tundra/core/TundraEngine.hpp>
#include <tundra/core/BinaryFileParser.hpp>

void generate_dummy_binary()
{
    std::ofstream out_file_stream;

    out_file_stream.open("dummy_binary", std::ios::out);
    out_file_stream.close();

    out_file_stream.open("dummy_binary", std::ios::binary);

    for(uint8_t i = 1; i < 41; ++i)    
    {
        out_file_stream << i;
    }
}


int main()
{
    // Tundra::TundraEngine t_engine;

    // t_engine.start();

    generate_dummy_binary();

    Tundra::BinaryFileParser bin_pars;

    if(!bin_pars.open_file("dummy_binary", 5))
    {
        std::cout << "[ERR] Failed to open binary file\n";
        exit(-1);
    }

    std::cout << "Reading through fetched pointer:\n";

    const uint8_t* bytes = bin_pars.read_n_bytes(20);

    for(int i = 0; i < 20; ++i)
    {
        std::cout << static_cast<int>(bytes[i]) << '\n';
    }

    std::cout << "\nReading in bytes directly:\n";

    while(!bin_pars.eof())
    {
        std::cout << static_cast<int>(bin_pars.read_byte()) << '\n';
    }

    std::cout << "\nReading entire file:\n";

    bytes = bin_pars.read_entire_open_file();

    for(int i = 0; i < bin_pars.get_file_byte_size(); ++i)
    {
        std::cout << static_cast<int>(bytes[i]) << '\n';
    }

    bin_pars.close_file();

    return 0;
}