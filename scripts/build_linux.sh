# !/bin/bash

BUILD_DIR=build

DECOMPILE_BUILD=OFF
OPTIMIZE_LEVEL=2
SANITIZER=ON

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        -clear)
            clear
            ;;
        -O0)
            OPTIMIZE_LEVEL=0
            ;;
        -no-san)
            SANITIZER=OFF
            ;;
    esac
done

cmake -G "Ninja" -B $BUILD_DIR -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DTUNDRA_OPTIMIZE_LEVEL="${OPTIMIZE_LEVEL}" \
    -DTUNDRA_ENABLE_SANITIZER="${SANITIZER}"
sudo cmake --build $BUILD_DIR --target install

