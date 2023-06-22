#!/bin/bash
output_file="weak_result.txt"
if [ -f "$output_file" ]; then
    rm "$output_file"
fi
echo Installo le dipendenze e compilo il codice..
cd ../source
chmod +x install_dependency_and_compile.sh
./install_dependency_and_compile.sh


path_file_test="../file_di_testo/file_benchmark"
file=8

if [ ! -d "$path_file_test/$file" ]; then
    echo 
    echo Lancia prima lo script word_generator_script.sh che si trova in utilities
else
    for ((num_processors = 2; num_processors <= 24; num_processors += 2))
    do
        echo "Esecuzione con $num_processors processori"
        echo "Esecuzione con $num_processors processori" >> "$output_file"
        
        echo "Prima run" >> "$output_file"
        mpirun -np $num_processors --allow-run-as-root --oversubscribe wordCount.out $path_file_test/$file >> "$output_file"
        wait
        rm word_count.csv 
        echo "Seconda run" >> "$output_file"
        mpirun -np $num_processors --allow-run-as-root --oversubscribe wordCount.out $path_file_test/$file >> "$output_file"
        wait
        rm word_count.csv 
        echo "Terza run" >> "$output_file"
        mpirun -np $num_processors --allow-run-as-root --oversubscribe wordCount.out $path_file_test/$file >> "$output_file"
        wait
        rm word_count.csv 
        echo "=======================" >> "$output_file"  # Aggiunge una riga vuota nel file di output
        file=$((file + 8))
        
    done
    mv $output_file ../$output_file
fi
