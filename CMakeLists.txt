
cmake_minimum_required(VERSION 3.10)
project(Tundra VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED)

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
    -O2
)

# Define link flags
set(COMMON_LINK_FLAGS
)

# If we are building to decompile, don't include sanitizers and don't link to 
# libc.
if(TUNDRA_DECOMPILE_BUILD STREQUAL "ON")
    message(STATUS "Building library for decompilation - no sanitizers or \
libc.")
    list(APPEND COMMON_COMPILE_FLAGS -nostdlib -fno-stack-protector -DTUNDRA_NOLIBC -fno-builtin)
    list(APPEND COMMON_LINK_FLAGS -nostdlib -fno-stack-protector)
else()
    message(STATUS "Building library")
    list(APPEND COMMON_COMPILE_FLAGS -fsanitize=address,undefined -g)
    list(APPEND COMMON_LINK_FLAGS -fsanitize=address,undefined)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_COMPILE_FLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${COMMON_LINK_FLAGS})

# Include directories 
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
install (DIRECTORY include/tundra DESTINATION include)

# Export target info for find_package()
install(EXPORT TundraTargets
    FILE TundraTargets.cmake
    NAMESPACE Tundra::
    DESTINATION lib/cmake/Tundra)

# Generate config + version files
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
