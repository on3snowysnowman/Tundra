
#include "tundra/utils/containers/LinkedList.hpp"

#include <cstdio>

int main()
{       
    Tundra::LnkLst::LinkedList<int> list;

    Tundra::LnkLst::init(&list);

    for(int i = 0; i < 10; ++i)
    {
        Tundra::LnkLst::add_to_end(&list, i);
    }

    Tundra::LnkLst::insert(&list, 4, 2);

    const Tundra::LnkLst::Node<int> *node = 
        Tundra::LnkLst::front(&list);

    while((bool)node)
    {
        printf("%d\n", node->value);
        node = Tundra::LnkLst::next(&list, node);
    }

    Tundra::LnkLst::free(&list);

    return 0;
}