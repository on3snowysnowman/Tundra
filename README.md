
# TUNDRA 

Tactical Utilities for Native Design and Real-Time Applications

![TUNDRA icon](assets/TUNDRA_Icon.png)

## Introduction

**TUNDRA** is a 2D game/data interaction engine designed to be lightweight and 
minimal in external dependencies. It is a passion project intended as a learning 
platform for writing clean, efficient, close-to-hardware C++ code.

## Project Goals

TUNDRA is *not* intended to be the next Unity — it's a hands-on educational 
project. The engine is currently under active development, though progress may 
be intermittent as I balance school (Computer Engineering) with coding.

This is the **4th edition** of the Engine, following:
- **OJAE**
- **LSDLE**
- **FROST**
- **TUNDRA** (the most efficient and ambitious so far)

The goal is to write as much of the engine as possible myself, minimizing 
third-party libraries. Some external libraries are used, such as GLFW and 
FreeType(No way I'm writing a font loader right now).

## Documentation

Full documentation is available here: [https://on3snowysnowman.github.io/Tundra/](https://on3snowysnowman.github.io/Tundra/)

## Features
 
There aren't many features implemented yet, but the goal is to support:

- 2D sprite rendering
- Input handling
- Modular state management
- Entity-Component-System (ECS) architecture
- Basic but extendable UI
- Simple file parsing and asset management

## Dependencies

TUNDRA requires these systems to be installed and locatable by cmake:
 - [GLFW](https://www.glfw.org/) Window and context management
 - [FreeType](https://freetype.org/) Font loading
 - [ZLib](https://zlib.net/) Image decompression

GLAD is bundled and compiled with the engine — no need to install it separately.

## Installation 

TUNDRA uses **CMake** as its build system and expects the GLFW and FreeType 
libraries to be installed system-wide.

### Prerequisities

Ensure the following are installed:

- CMake (version 3.10 or higher)
- C++ compiler (G++, Clang++, MSVC, etc.)
- GLFW installed and discoverable by CMake
- FreeType installed and discoverable by Cmake
- ZLib installed and discoverable by Cmake

### Build Instructions

Platform-specific build scripts are located in the `/scripts` directory.

To build and install TUNDRA on your platform, run the corresponding script:

```bash
./scripts/platform_build_script
```

Once the script completes, TUNDRA will be compiled and installed to the default
install paths specified by cmake per platform.

### Usage 

To use the Tundra library, simply include:

```bash
find_package(Tundra REQUIRED)

target_link_libraries(TARGET_HERE Tundra::Tundra)
```

in your CMakeLists.txt file.

More documentation on the "ins and outs" of the Engine will be added later as 
more tools are written.

## Contact 

Name: Joel Height

Email: On3SnowySnowman@gmail.com
