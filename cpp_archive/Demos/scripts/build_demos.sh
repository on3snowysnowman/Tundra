# #!/bin/bash

# Initialize flags
clear_terminal=false
decompile=false

BUILD_DIRECTORY=build

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        -clear)
            clear_terminal=true
            ;;
        -decompile)
            decompile=true
            ;;
        *)
            echo "Unknown argument: $arg"
            ;;
    esac
done

# Clear terminal if requested
if $clear_terminal; then
    clear
fi

cd ..

if $decompile; then

    # Execute Engine's build script.
    ./scripts/build_linux.sh -decompile

    cd Demos

    cmake -B $BUILD_DIRECTORY -S . -G "Ninja" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_FOR_DECOMPILE=ON

    cmake --build $BUILD_DIRECTORY 

    objdump -d bin/demo | c++filt > asm/demo.s

else

    ./scripts/build_linux.sh 
    
    cd Demos

    cmake -B $BUILD_DIRECTORY -S . -G "Ninja" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_FOR_DECOMPILE=OFF

    cmake --build $BUILD_DIRECTORY 
fi
