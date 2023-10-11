#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Insufficient number of arguments"
    exit 1
fi

count=0
sum=0

file=$1
dir=$2
shift 2

for arg in "$@"; do
        if [ $arg -gt 10 ]; then
            count=$(($count+1))
        fi
        sum=$(($sum+$arg))
done

if [ ! -e $file ]; then
    echo "count = $count; sum = $sum" > $file
else
    if [ -f $file ]; then
    echo "count = $count; sum = $sum" > $file
    else
    echo "Invalid file"
    exit 1
    fi
fi

nr_digits=0
nr_digits=`expr $sum | wc -L`

echo "nr_digits = $nr_digits"

if [ ! -d $dir ]; then
    echo "Invalid directory"
    exit 1
fi

for fisier in "$dir"/*.txt; do
    if [ ! -f $fisier ]; then
        echo "Invalid file: $fisier"
        exit 1
    fi

    cat $fisier
done
