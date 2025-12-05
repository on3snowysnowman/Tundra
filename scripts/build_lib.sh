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

cmake -G "Ninja" -B $BUILD_DIR -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
sudo cmake --build $BUILD_DIR --target install
