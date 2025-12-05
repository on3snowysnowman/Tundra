
cmake_minimum_required(VERSION 3.10)

project(TundraContainerGen)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define compile flags
set(COMMON_COMPILE_FLAGS
    -g
    -Wall
    -Wextra
    -march=native
    -fsanitize=address,undefined
)

# Define link flags
set(COMMON_LINK_FLAGS
    # -nostdlib
    -fsanitize=address,undefined
)

find_package(Tundra REQUIRED)

# Glob all generators.
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(SRC_FILE ${SOURCES})

    # Extract file name without extension to use as target name
    get_filename_component(SRC_NAME ${SRC_FILE} NAME_WE)

    # Create executable.
    add_executable(${SRC_NAME} ${SRC_FILE})

    target_compile_options(${SRC_NAME} PRIVATE ${COMMON_COMPILE_FLAGS})
    target_link_options(${SRC_NAME} PRIVATE ${COMMON_LINK_FLAGS})
    
    # Include directories 
    target_include_directories(${SRC_NAME}
        PUBLIC
            /usr/local/include
            /usr/include
    )

    # Link Demo to the Engine
    target_link_libraries(${SRC_NAME}
    PRIVATE
        Tundra::Tundra
    )
endforeach()
