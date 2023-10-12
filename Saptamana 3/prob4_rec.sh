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

for entity in "$director"/*; do
    if [ -f "$entity" ] && [[ "$entity" == *.txt ]]; then
        case "$caracter" in
            "r") chmod +r "$entity" ;;
            "w") chmod +w "$entity" ;;
            "x") chmod +x "$entity" ;;
            *)
                echo "Invalid char"
                exit 1
                ;;
        esac
        echo "Rights for $entity set to $caracter"
    elif [ -d "$entity" ]; then
        bash $0 $entity $caracter
    fi
done

