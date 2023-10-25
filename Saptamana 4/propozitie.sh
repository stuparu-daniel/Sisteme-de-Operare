#!/bin/bash

if [ $# -ne 0 ]; then
    echo "Incorrect usage"
    exit 1
fi


while read linie
do
  echo $linie | grep -E "^[A-Z][a-zA-Z0-9 ,]+\.$" | grep -E -v "si[ ]*\," | grep -E -v "n[pb]"
done

