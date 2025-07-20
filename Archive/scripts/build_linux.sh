
# Clear terminal.
if [ "$1" = "-clear" ]; then
    clear
fi

cmake -G "Ninja" -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
sudo cmake --build build --target install