
cmake_minimum_required(VERSION 3.10)

project(TundraTests)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define compile flags
set(COMMON_COMPILE_FLAGS
    -g
    -Wall
    -Wextra
    -march=native
    -fsanitize=address,undefined
    -std=gnu99
)

# Define link flags
set(COMMON_LINK_FLAGS
    # -nostdlib
    -fsanitize=address,undefined
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
    
    # Include directories 
    target_include_directories(${TEST_NAME}
        PUBLIC
            /usr/local/include
            /usr/include
    )

    # Link Demo to the Engine
    target_link_libraries(${TEST_NAME}
    PRIVATE
        Tundra::Tundra
    )
endforeach()
