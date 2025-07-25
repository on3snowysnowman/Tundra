
# TUNDRA 

Tactical Utilities for Native Design and Real-Time Applications

![TUNDRA icon](assets/TUNDRA_Icon.png)

## Introduction

**TUNDRA** is a 2D game/data interaction engine designed to be lightweight and 
minimal in external dependencies. It is a passion project intended as a learning 
platform for writing clean and efficient code.

The engine is written in a restricted subset of C++, adhering to a C-style 
coding discipline. It avoids classes, exceptions, RTTI, and the C++ standard 
library, relying only on features that provide zero runtime cost abstractions, 
such as templates, constexpr, and namespaces.

## Project Goals

TUNDRA is *not* intended to be the next Unity — it's a hands-on educational 
project. The engine is currently under active development, though progress may 
be intermittent as I balance school (Computer Engineering) with coding.

This is the **fourth iteration** of the Engine, following:
- **OJAE**
- **LSDLE**
- **FROST**
- **TUNDRA** (the most efficient and ambitious so far)

The goal is to write as much of the engine as possible myself.

## Features
 
There aren't many features implemented yet, but the goal is to support:

- 2D sprite rendering
- Input handling
- Modular state management
- Entity-Component-System (ECS) architecture
- Basic but extendable UI
- Simple file parsing and asset management

## Dependencies

As of now, TUNDRA requires no dependencies, and is standalone. This may change 
in the future. 

A 64-bit system with GCC or Clang is REQUIRED to compile the engine. 
MSVC is not supported as it does not allow inline assembly.

## Installation 

TUNDRA uses **CMake** as its build and installation system.

### Prerequisites

Ensure the following are installed:

- CMake (version 3.10 or higher)
- C++ compiler (G++, Clang++, MSVC, etc.)

### Build Instructions

Platform-specific build scripts are located in the `/scripts` directory.

To build and install TUNDRA on your platform, run the corresponding script:

```bash
./scripts/platform_build_script
```

Once the script completes, TUNDRA will be compiled and installed to the default
install paths specified by cmake per platform.

As of now, only a linux build script exists, but I plan to add the others in 
the future.

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
