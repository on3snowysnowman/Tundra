
cmake_minimum_required(VERSION 3.10)
project(Tundra_lib)

# Place demo binary in the source folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# Find required packages.
find_package(glfw3 REQUIRED)

# Collect all cpp files from the src/ directory.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the static library with the globbed cpp source files.
add_library(${PROJECT_NAME} STATIC ${SOURCES} src/glad.c)

# Specify include directories for the static library.
target_include_directories(${PROJECT_NAME}
PUBLIC
    # TundraEngine
    ${CMAKE_SOURCE_DIR}/include/core

    # Third Party
    ${CMAKE_SOURCE_DIR}/include/thirdparty
    /usr/include
    /usr/local/include
PRIVATE
    # TundraEngine Internal
    ${CMAKE_SOURCE_DIR}/include/internal
)

# Add demo executable.
add_executable(main main.cpp)

# Specify include directories for the demo.
target_include_directories(main
PUBLIC
    ${CMAKE_SOURCE_DIR}/include/core
)

# Link demo to the static library.
target_link_libraries(main
PRIVATE
    ${PROJECT_NAME}
    glfw
)
