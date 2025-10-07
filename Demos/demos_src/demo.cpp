
// #include "tundra/utils/containers/LinkedList.hpp"

#include "tundra/utils/StringConversion.hpp"
#include "tundra/utils/CoreTypes.hpp"

#include <iostream>

int main()
{      
    // Tundra::int8 num8;

    Tundra::Str::String str; 
    Tundra::Str::init(&str, "-18446744073709551617", 21);

    // num8 = Tundra::string_to_num<Tundra::int8>(&str);

    // std::cout << int(num8) << '\n';

    // Tundra::uint8 numu8 = Tundra::string_to_num<Tundra::uint8>(&str);
    
    // std::cout << int(numu8) << '\n';

    // Tundra::int16 num16 = Tundra::string_to_num<Tundra::int16>(&str);

    // std::cout << num16 << '\n';

    // Tundra::uint16 numu16 = Tundra::string_to_num<Tundra::uint16>(&str);

    // std::cout << numu16 << '\n';

    // Tundra::int32 num32 = Tundra::string_to_num<Tundra::int32>(&str);

    // std::cout << num32 << '\n';

    // Tundra::uint32 numu32 = Tundra::string_to_num<Tundra::uint32>(&str);

    // std::cout << numu32 << '\n';

    Tundra::int64 num64 = Tundra::string_to_num<Tundra::int64>(&str);

    std::cout << num64 << '\n';

    // Tundra::uint64 numu64 = Tundra::string_to_num<Tundra::uint64>(&str);

    // std::cout << numu64 << '\n';

    Tundra::Str::free(&str);
    
    return 0;
}