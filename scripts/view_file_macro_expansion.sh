
if [ -z "$1" ]; then
    echo "No input file provided."
    exit 1
fi

gcc -E "$1" -o "$1"_mac_expanded.c
