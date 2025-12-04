# !/bin/bash

BUILD_DIR=build

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        -clear)
            clear
            ;;
    esac
done

# Build library 
cd ..
./scripts/build_linux.sh

cd Demos

# Build Demos
cmake -G "Ninja" -B $BUILD_DIR -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build $BUILD_DIR
