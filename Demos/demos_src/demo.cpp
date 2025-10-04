
// #include "tundra/utils/containers/LinkedList.hpp"

#include "tundra/utils/StringConversion.hpp"
#include "tundra/utils/CoreTypes.hpp"

#include <iostream>

int main()
{      

    Tundra::int8 num;

    Tundra::Str::String str; 
    Tundra::Str::init(&str, "-104", 4);

    num = Tundra::string_to_num<Tundra::int8>(&str);
    
    Tundra::Str::free(&str);

    std::cout << num << '\n';
    
    return 0;
}