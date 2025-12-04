# #!/bin/bash

# Initialize flags
clear_terminal=false
decompile=false

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

if $decompile; then

    # Build the library with decompiling in mind, stripping address sanitization 
    # and any optimizations.

    cmake -G "Ninja" -B build -S . -DBUILD_FOR_DECOMPILE=ON \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    sudo cmake --build build --target install

else

    cmake -G "Ninja" -B build -S . -DBUILD_FOR_DECOMPILE=OFF \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    sudo cmake --build build --target install
fi

