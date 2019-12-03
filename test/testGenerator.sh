#!/bin/bash
if [ "$#" -lt 1 ]; then
    echo "Usage: ./testGenerator.sh <PROC_NUM>"
    exit
fi
declare -i initTime num
declare -i file num
file=$(ls -1 | wc -l)
initTime=0
for ((i = 1; i <= $1; i++)) do
    echo "Iteración número $i"
    initTime+=$(shuf -i 0-2 -n 1)
    name=$(shuf -i 1-9999 -n 1)
    procMem=$(shuf -i 100-500 -n 1)
    procTime=$(shuf -i 1-5 -n 1)
    echo "$initTime $name $procMem $procTime" >> $file.txt
    sleep 0.1 #Para que genere bien nums aleatorios
done
