#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <dir> <file_out>"
    exit 1
fi

dir=$1
file_out=$2
total=0

if [ -e "$file_out" ]; then
    if [ ! -f "$file_out" ]; then
        echo "Corrupted existing output file $file_out"
        exit 1
    fi
else
    touch $file_out
fi

for fisier in "$dir"/*.txt; do
    if [ -f "$fisier" ]; then
        chars_per_file=$(wc -m < "$fisier")
        total=$((total+"$chars_per_file"))
        echo "File name:$fisier , word count: $chars_per_file" >> "$file_out"
    fi
done

echo "Total word count across all files in $dir is $total" >> "$file_out"

