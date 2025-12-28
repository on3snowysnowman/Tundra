/**
 * @file TestingHelper.hpp
 * @author Joel Height (you@domain.com)
 * @brief Simple helper for registering and running tests.
 * @date 2025-11-23
 * 
 * @copyright Copyright (c) 2025
*/

#pragma once

#include <vector>
#include <iostream>

#include "tundra/Tundra.h"
#include "tundra/internal/MacroHelper.h"

using TestFunc = void(*)(void);

inline std::vector<TestFunc> all_tests;

struct TestRegistrar {
    explicit TestRegistrar(TestFunc func) 
    {
        all_tests.push_back(func);
    }
};

#define TEST_BEGIN(name)                                 \
    void test_##name(void);                              \
    static TestRegistrar registrar_##name(test_##name);  \
    void test_##name(void)                               \
    {                                                    \
        std::cout << "test_"#name": \n";                 \
        std::cout.flush();

#define TEST_END                                         \
        std::cout << "Pass!" << std::endl;               \
    }

#define TEST_MAIN(test_name) \
    int main(){std::cout<<" -- " TUNDRA_STRINGIFY(test_name)" --\n";\
    assert(Tundra_init()==0);run_all_tests();std::cout<<'\n';return 0;}

void run_all_tests()
{
    for(int i = 0; i < all_tests.size(); ++i)
    {
        all_tests.at(i)();
    }

    std::cout << " -- All tests passed --\n";
}
