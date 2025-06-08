
#include <iostream>

#include "tundra/core/BinaryFileParser.hpp"
#include "tundra/core/PNGLoader.hpp"

void read_entire_file(const char* png_path)
{
    Tundra::BinaryFileParser bin_parser;

    bin_parser.open_file(png_path);

    uint8_t current_output_num = 0;

    std::cout << "Entire PNG:\n";

    while(!bin_parser.eof())
    {
        uint8_t parsed_byte = bin_parser.read_byte();

        if(parsed_byte < 10)
        {
            std::cout << "..";
        }

        else if(parsed_byte < 100)
        {
            std::cout << '.';
        }

        std::cout << int(parsed_byte) << ' ';

        if(++current_output_num > 27)
        {
            std::cout << '\n';
            current_output_num = 0;
        }
    }

    std::cout << "\n\n";

    bin_parser.close_file();
}


int main()
{
    // const char* png_path = "assets/TUNDRA_Icon.png";
    const char* png_path = "assets/dummy_png.png";

    // read_entire_file(png_path);

    std::cout << "PNG parser with PNGLoader:\n";

    Tundra::PNGLoader png_loader;

    png_loader.load_png(png_path);

    return 0;
}
