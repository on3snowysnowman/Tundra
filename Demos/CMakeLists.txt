
cmake_minimum_required(VERSION 3.10)
project(TundraDemos VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED)

message(STATUS " == BUILDING DEMOS == ")

# Collect all .c files from the src directory.
file(GLOB_RECURSE SOURCES "src/*.c")

# Create static library 
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Define compile flags
set(COMMON_COMPILE_FLAGS
    -Wall
    -Wextra
    -Wstrict-prototypes
    -Werror=implicit-function-declaration
    -Wmissing-prototypes
    -march=native
    -g
)

# Define link flags
set(COMMON_LINK_FLAGS
)

# Optimization enabling
if(TUNDRA_OPTIMIZE_LEVEL STREQUAL "0")
    message(STATUS "Building with no compiler optimizations.")
    list(APPEND COMMON_COMPILE_FLAGS -O0)
else()
    list(APPEND COMMON_COMPILE_FLAGS -O2)
endif()

# Sanitizer enabling
if(TUNDRA_ENABLE_SANITIZER)
    message(STATUS "Building with sanitizers enabled.")
    list(APPEND COMMON_COMPILE_FLAGS -fsanitize=address,undefined)
    list(APPEND COMMON_LINK_FLAGS -fsanitize=address,undefined)
else()
    message(STATUS "Building with sanitizers disabled.")
    list(APPEND COMMON_COMPILE_FLAGS -fno-stack-protector)
endif()

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
