# !/bin/bash

BUILD_DIR=build

DECOMPILE_BUILD=OFF

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        -clear)
            clear
            ;;
        -decompile)
            DECOMPILE_BUILD=ON
            ;;
    esac
done

cmake -G "Ninja" -B $BUILD_DIR -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DTUNDRA_DECOMPILE_BUILD="${DECOMPILE_BUILD}"
sudo cmake --build $BUILD_DIR --target install

