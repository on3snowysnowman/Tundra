
#include "tundra/utils/containers/LinkedList.hpp"

int main()
{       
    Tundra::LnkLst::LinkedList<int> list;

    Tundra::LnkLst::init(&list);

    for(int i = 0; i < 10; ++i)
    {
        Tundra::LnkLst::add_to_end(&list, i);
    }

    Tundra::LnkLst::insert(&list, 4, 2);

    Tundra::LnkLst::free(&list);

    return 0;
}