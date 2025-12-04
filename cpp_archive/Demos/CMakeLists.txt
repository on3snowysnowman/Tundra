
cmake_minimum_required(VERSION 3.10)

project(TundraDemos)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Base compiler flags (common)
set(COMMON_COMPILE_FLAGS
    -fno-exceptions
    -fno-rtti
    -O0
    -g
    -Wall
    -Wextra
    -march=native
)

set(COMMON_LINK_FLAGS
    # -nostdlib
    # -nostdlib++
)

# add_compile_definitions(TUNDRA_USE_C_MALLOC)

# Conditionally add sanitizers
if(DEFINED BUILD_FOR_DECOMPILE AND BUILD_FOR_DECOMPILE STREQUAL "ON")
    message(STATUS "Demos: Building for decompilation — no sanitizers.")
else()
    list(APPEND COMMON_COMPILE_FLAGS -fsanitize=address,undefined
)
    list(APPEND COMMON_LINK_FLAGS -fsanitize=address,undefined)
endif()

find_package(Tundra REQUIRED)

# Glob all demos.
file(GLOB DEMO_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(DEMO_FILE ${DEMO_SOURCES})
    
    # Extract file name without extension to use as target name
    get_filename_component(DEMO_NAME ${DEMO_FILE} NAME_WE)

    # Create executable.
    add_executable(${DEMO_NAME} ${DEMO_FILE})

    target_compile_options(${DEMO_NAME} PRIVATE ${COMMON_COMPILE_FLAGS})
    target_link_options(${DEMO_NAME} PRIVATE ${COMMON_LINK_FLAGS})
    
    # Link Demo to the Engine
    target_link_libraries(${DEMO_NAME}
    PRIVATE
        Tundra::Tundra
    )
endforeach()
