
cmake_minimum_required(VERSION 3.10)
project(Tundra VERSION 1.0)

# Place demo binary in the source folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Collect all cpp files from the src/ directory.
file(GLOB_RECURSE SOURCES "src/*.c")

# Create the static library with the globbed c source files.
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE 
    -std=c99 -O0 -fsanitize=address,undefined -g -Wall -Wextra -pedantic
)

# target_compile_options(${PROJECT_NAME} PRIVATE 
#     -std=c99 -O0
# )

# Create alias
add_library(Tundra::Tundra ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    /usr/local/include
    /usr/include
)

# Install lib
install(TARGETS Tundra 
    EXPORT TundraTargets
    ARCHIVE)

# Install headers
install(DIRECTORY include/tundra DESTINATION include)

# Export target info for find_package()
install(EXPORT TundraTargets
    FILE TundraTargets.cmake
    NAMESPACE Tundra::
    DESTINATION lib/cmake/Tundra
)

# Generate Config + Version files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/TundraConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TundraConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/TundraConfig.cmake"
    INSTALL_DESTINATION lib/cmake/Tundra
)

# Install the config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/TundraConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/TundraConfigVersion.cmake"
    DESTINATION lib/cmake/Tundra
)
