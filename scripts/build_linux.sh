
# Clear terminal.
if [ "$1" = "-clear" ]; then
    clear
fi

cmake -G "Ninja" -S . -B build
sudo cmake --build build --target install