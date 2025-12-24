
cmake_minimum_required(VERSION 3.10)
project(TundraDemos VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED)

# Collect all .c files from the src directory.
file(GLOB_RECURSE SOURCES "src/*.c")

# Create static library 
add_library(${PROJECT_NAME} STATIC ${SOURCES})

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

# Glob all demos.
file(GLOB DEMO_SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(DEMO_FILE ${DEMO_SOURCES})
    
    # Extract file name without extension to use as target name
    get_filename_component(DEMO_NAME ${DEMO_FILE} NAME_WE)

    # Create executable.
    add_executable(${DEMO_NAME} ${DEMO_FILE})

    target_compile_options(${DEMO_NAME} PRIVATE ${COMMON_COMPILE_FLAGS})
    target_link_options(${DEMO_NAME} PRIVATE ${COMMON_LINK_FLAGS})
    
    # Link Demo to the Library
    target_link_libraries(${DEMO_NAME}
    PRIVATE
        Tundra::Tundra
    )
endforeach()
