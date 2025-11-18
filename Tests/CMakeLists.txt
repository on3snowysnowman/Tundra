
cmake_minimum_required(VERSION 3.10)

project(TundraTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_COMPILE_FLAGS 
    -g
    -Wall
    -Wextra
    -march=native
)

find_package(Tundra REQUIRED)

# Glob all tests.
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(TEST_FILE ${TEST_SOURCES})

    # Extract file name without extension to use as target name
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # Create executable.
    add_executable(${TEST_NAME} ${TEST_FILE})

    target_compile_options(${TEST_NAME} PRIVATE ${COMMON_COMPILE_FLAGS})
    target_link_options(${TEST_NAME} PRIVATE ${COMMON_LINK_FLAGS})
    
    # Link Demo to the Engine
    target_link_libraries(${TEST_NAME}
    PRIVATE
        Tundra::Tundra
    )
endforeach()
