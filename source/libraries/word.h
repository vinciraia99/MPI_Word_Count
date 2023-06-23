#include <string.h>
#include <stdlib.h>
#include "mpi.h"

#define MAXSTRING 48

typedef struct Word_occurrence{

    int number_repeats;
    char word[MAXSTRING];

} Word_occurrence;

Word_occurrence * create_word_occurence(char * word ,int num);

void create_word_datatype(MPI_Datatype *wordtype);

void sort_occurrences(Word_occurrence ** p_occurences, int n);