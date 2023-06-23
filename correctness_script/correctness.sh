#!/bin/bash
file_out="word_count.out"
dir_text="../file_di_testo"
cd ../source

mpirun -np 2 --allow-run-as-root $file_out ./$dir_text

cp word_count.csv oracolo.csv

for i in {5..60..5}
do
    echo "Verifica correttezza con $i processori"

    mpirun -np $i --allow-run-as-root --oversubscribe $file_out ./$dir_text

    echo
    if [[ $(diff <(sort word_count.csv) <(sort oracolo.csv)) ]]; then
        echo "Differenze trovate!"
        exit 1
    else
        echo "Differenze non trovate."
    fi
    echo
    rm word_count.csv
done

rm oracolo.csv




