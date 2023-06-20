#!/bin/bash

# Compila wordGenerator.c
gcc -o wordGenerator.out wordGenerator.c

# Loop per generare e lanciare i file con grandezza crescente
file_size=8
file_benchmark="file_benchmark"
if [ -d "../file_di_testo/$file_benchmark" ]; then
    rm -r ../file_di_testo/$file_benchmark/
fi
if [ ! -d "../file_di_testo/$file_benchmark" ]; then
        mkdir ../file_di_testo/$file_benchmark/
fi
while [ $file_size -le 184 ]
do
    file_name="${file_size}MB.txt"
    ./wordGenerator.out $file_size $file_name
    mkdir ../file_di_testo/$file_benchmark/$file_size
    mv $file_name ../file_di_testo/$file_benchmark/$file_size
    file_size=$((file_size + 8))
done
if [ -d "../file_di_testo/strong_benchmark" ]; then
    rm -r ../file_di_testo/strong_benchmark
fi
if [ ! -d "../file_di_testo/strong_benchmark" ]; then
        mkdir ../file_di_testo/strong_benchmark
fi
./wordGenerator.out 512 strong.txt
mv strong.txt ../file_di_testo/strong_benchmark/strong.txt
rm wordGenerator.out
