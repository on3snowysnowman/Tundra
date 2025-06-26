#!/bin/bash

BUILD_DIRECTORY="build"

# Clear terminal.
if [ "$1" = "-clear" ]; then
    clear
fi

cd ../

# Execute Engine's build script.
./scripts/build_linux.sh

cd Demos

if [ ! -d $BUILD_DIRECTORY ]; then

    echo "Build directory not detected, creating..."
    mkdir $BUILD_DIRECTORY
fi

echo "Configuring Demo build"

# Configure cmake
cmake -B $BUILD_DIRECTORY -S . -G "Ninja"

echo "Building Demos"

# Build Demos
cmake --build $BUILD_DIRECTORY


