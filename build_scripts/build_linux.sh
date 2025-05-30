#!/bin/bash

# Specify the build directory.
BUILD_DIRECTORY="build"

# If the build directory does not exist.
if [ ! -d $BUILD_DIRECTORY ]; then

    echo "$BUILD_DIRECTORY does not exist."
    mkdir "$BUILD_DIRECTORY"
    chmod 777 $BUILD_DIRECTORY

    echo "Created directory"
fi

echo "Configuring Engine build."

# Configure cmake for the Engine build.

cmake -B $BUILD_DIRECTORY -S . -G "Ninja"

echo "Building Engine."

# Build Engine

# sudo cmake --build $BUILD_DIRECTORY --target install

cmake --build $BUILD_DIRECTORY

# ./install_dependencies.sh