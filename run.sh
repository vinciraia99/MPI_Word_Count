#!/bin/bash
code="word_count.out"
dir_text="../file_di_testo"
file_out="word_count.csv"
cd source
echo Installo le dipendenze e comnpilo il codice...
echo
chmod +x install_dependency_and_compile.sh
./install_dependency_and_compile.sh
echo
echo Eseguo un semplice lancio del codice
mpirun -np 2 --allow-run-as-root $code ./$dir_text
echo Codice eseguito! Ecco la soluzione qui sotto che è disponibile sottoforma di csv nella cartella source
echo
cat $file_out
echo




