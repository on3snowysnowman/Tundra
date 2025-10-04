
// #include "tundra/utils/containers/LinkedList.hpp"

#include "tundra/utils/StringConversion.hpp"
#include "tundra/utils/CoreTypes.hpp"

#include <cstdio>

int main()
{      
    Tundra::int8 num = 127;

    Tundra::Str::String str = Tundra::num_to_string(num);

    printf("%s\n", Tundra::Str::data(&str));

    Tundra::Str::free(&str);
    
    // Tundra::LnkLst::LinkedList<int> list;

    // Tundra::LnkLst::init(&list);

    // for(int i = 0; i < 10; ++i)
    // {
    //     Tundra::LnkLst::add_to_end(&list, i);
    // }

    // Tundra::LnkLst::insert(&list, 4, 2);

    // const Tundra::LnkLst::Node<int> *node = 
    //     Tundra::LnkLst::front(&list);

    // while((bool)node)
    // {
    //     printf("%d\n", node->value);
    //     node = Tundra::LnkLst::next(&list, node);
    // }

    // Tundra::LnkLst::free(&list);

    return 0;
}