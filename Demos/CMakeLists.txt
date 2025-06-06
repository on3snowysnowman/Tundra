
cmake_minimum_required(VERSION 3.10)

project(TundraDemos)

find_package(Tundra REQUIRED)
# find_package(glfw3 REQUIRED)
# find_package(Freetype REQUIRED)

# Glob all demos.
file(GLOB DEMO_SOURCES ${CMAKE_SOURCE_DIR}/demos_src/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(DEMO_FILE ${DEMO_SOURCES})
    
    # Extract file name without extension to use as target name
    get_filename_component(DEMO_NAME ${DEMO_FILE} NAME_WE)

    # Create executable.
    add_executable(${DEMO_NAME} ${DEMO_FILE})
    
    # Link Demo to the Engine
    target_link_libraries(${DEMO_NAME}
    PRIVATE
        Tundra::Tundra
    )
endforeach()
