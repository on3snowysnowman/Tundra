# !/bin/bash

BUILD_DIR=build

DECOMPILE_BUILD=OFF
OPTIMIZE_LEVEL=2
SANITIZER=ON

lib_script_args=()

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        -clear)
            clear
            ;;
        -O0)
            OPTIMIZE_LEVEL=0
            lib_script_args+=(-O0)
            ;;
        -no-san)
            SANITIZER=OFF
            lib_script_args+=(-no-san)
            ;;
        -decompile)
            DECOMPILE_BUILD=ON
            ;;
    esac
done

echo --LIBRARY BUILD--

# Build library 
cd ..
./scripts/build_linux.sh "${lib_script_args[@]}"

cd Demos

echo
echo --DEMOS BUILD--

# Build Demos
cmake -G "Ninja" -B $BUILD_DIR -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DTUNDRA_OPTIMIZE_LEVEL="${OPTIMIZE_LEVEL}" \
    -DTUNDRA_ENABLE_SANITIZER="${SANITIZER}"
cmake --build $BUILD_DIR

if [[ "$DECOMPILE_BUILD" == "ON" ]]; then
objdump -d -l bin/demo > asm/demo.s
echo Generated assembly file: asm/demo.s
fi

