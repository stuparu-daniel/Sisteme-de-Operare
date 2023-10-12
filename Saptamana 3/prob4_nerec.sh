#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <dir> <char>"
    exit 1
fi

director=$1
caracter=$2

if [ ! -d "$director" ]; then
    echo "Invalid folder"
    exit 1
fi

for fisier in "$director"/*.txt; do
    if [ -f "$fisier" ]; then
        case "$caracter" in
            "r") chmod +r "$fisier" ;;
            "w") chmod +w "$fisier" ;;
            "x") chmod +x "$fisier" ;;
            *)
                echo "Invalid char"
                exit 1
                ;;
        esac
        echo "Rights for $fisier set to $caracter"
    fi
done


