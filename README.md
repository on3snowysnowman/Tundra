
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

The goal is to write as much of the engine as possible in C++, minimizing 
third-party libraries. Some external libraries are used, such as GLFW and 
FreeType(No way I'm writing a font loader right now).

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
 - [FreeType](https://freetype.org/) Font loading.

GLAD is bundled and compiled with the engine — no need to install it separately.

## Installation 

**Installation process is not complete for the Engine yet, nothing will be 
installed to disk.**

TUNDRA uses **CMake** as its build system and expects GLFW to be installed 
system-wide.

### Prerequisities

Ensure the following are installed:

- CMake (version 3.10 or higher)
- C++ compiler (G++, Clang++, MSVC, etc.)
- GLFW installed and discoverable by CMake
- FreeType installed and discoverable by Cmake

### Install Paths

In the root directory, **create** a file called `InstallPaths.txt`. Edit this 
file to include **absolute paths** for:

1. Engine binary install path  
2. Header install path  
3. Assets install path  


**Example:**
```
/usr/lib
/usr/include
/home/username/Documents/tundra_assets
```

These paths are used during the install phase to place TUNDRA where your 
projects can find and link to it.

### Build Instructions

Platform-specific build scripts are located in the `/build_scripts` directory.

To build and install TUNDRA on your platform, run the corresponding script:

```bash
./build_scripts/platform_build_script
```

Once the script completes, TUNDRA will be compiled and installed to the paths 
you specified in `InstallPaths.txt`.

## Contact 

Name: Joel Height

Email: On3SnowySnowman@gmail.com
