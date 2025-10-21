
cmake_minimum_required(VERSION 3.10)
project(Tundra VERSION 1.0)

# Place demo binary in the source folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all cpp files from the src/ directory.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

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
    message(STATUS "Building for decompilation — no sanitizers.")
else()
    list(APPEND COMMON_COMPILE_FLAGS -fsanitize=address,undefined 
)
    list(APPEND COMMON_LINK_FLAGS -fsanitize=address,undefined)
endif()

# Apply to static library
target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_COMPILE_FLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${COMMON_LINK_FLAGS})

# Include directories (common)
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
