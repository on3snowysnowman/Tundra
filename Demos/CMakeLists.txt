
cmake_minimum_required(VERSION 3.10)

project(TundraDemos)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(Tundra REQUIRED)

# Glob all demos.
file(GLOB DEMO_SOURCES ${CMAKE_SOURCE_DIR}/demos_src/*.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(DEMO_FILE ${DEMO_SOURCES})
    
    # Extract file name without extension to use as target name
    get_filename_component(DEMO_NAME ${DEMO_FILE} NAME_WE)

    # Create executable.
    add_executable(${DEMO_NAME} ${DEMO_FILE})
    
    # Set compile options for this target
    target_compile_options(${DEMO_NAME} PRIVATE
        -std=c99 -O0 -fsanitize=address,undefined -g -Wall -Wextra -pedantic
    )

    target_link_options(${DEMO_NAME} PRIVATE -fsanitize=address,undefined)

    # Link Demo to the Engine
    target_link_libraries(${DEMO_NAME}
    PRIVATE
        Tundra::Tundra
    )
endforeach()
