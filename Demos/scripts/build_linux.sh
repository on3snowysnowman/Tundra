# !/bin/bash

BUILD_DIR=build

DECOMPILE_BUILD=OFF

lib_script_args=()

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        -clear)
            clear
            ;;
        -decompile)
            lib_script_args+=(-decompile)
            DECOMPILE_BUILD=ON
            ;;
    esac
done

echo --LIBRARY BUILD--

# Build library 
cd ..
./scripts/build_lib.sh "${lib_script_args[@]}"

cd Demos

echo
echo --DEMOS BUILD--

# Build Demos
cmake -G "Ninja" -B $BUILD_DIR -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DTUNDRA_DECOMPILE_BUILD="${DECOMPILE_BUILD}"
cmake --build $BUILD_DIR

if [[ "$DECOMPILE_BUILD" == "ON" ]]; then
echo Generated assembly file: asm/demo.s
objdump -d -l bin/demo > asm/demo.s
fi

