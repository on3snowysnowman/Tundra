
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
        // printf("Value: %d\nForward: %lld\nBackward: %lld\n\n", 
        //     node->value, node->next, node->previous);
        printf("%d\n", node->value);
        node = Tundra::LnkLst::next(&list, node);
    }

    // for(int i = 0; i < Tundra::LnkLst::size(&list); ++i)
    // {
    //     printf("%d: \nValue: %d\nForward: %lld\nBackward: %lld\n\n", i, 
    //         list.nodes[i].value, list.nodes[i].next, list.nodes[i].previous);
    // }

    Tundra::LnkLst::free(&list);

    return 0;
}