

BUILD_DIRECTORY=build

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        -clear)
            clear
            ;;
        *)
            echo "Unknown argument: $arg"
            ;;
    esac
done

cd ..

./scripts/build_linux.sh

cd Tests

cmake -B $BUILD_DIRECTORY -S . -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build $BUILD_DIRECTORY
