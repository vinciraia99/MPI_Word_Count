#!/bin/bash
c_file="word_generator.c"
out_file="word_generator.out"

gcc -o $out_file $c_file
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
    ./$out_file $file_size $file_name
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
./$out_file 512 strong.txt
mv strong.txt ../file_di_testo/strong_benchmark/strong.txt
rm $out_file
