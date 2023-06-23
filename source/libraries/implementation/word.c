#include "word.h"

Word_occurrence * create_word_occurence(char * word ,int num){

    Word_occurrence * w = malloc(sizeof(Word_occurrence));

    strncpy(w->word, word, MAXSTRING);
    w->number_repeats = num;

    return w;
}

void create_word_datatype(MPI_Datatype *wordtype){

    MPI_Datatype oldtypes[2];
    int blockcounts[2];

    MPI_Aint    offsets[2], lb, extent;

    offsets[0] = 0;
    oldtypes[0] = MPI_INT;
    blockcounts[0] = 1;

    MPI_Type_get_extent(MPI_INT, &lb, &extent);
    offsets[1] = 1 * extent;
    oldtypes[1] = MPI_CHAR;
    blockcounts[1] = MAXSTRING;


    MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, wordtype);
    MPI_Type_commit(wordtype);


}

//funzione per lo swap degli elementi
void swap(Word_occurrence *a, Word_occurrence *b) {
  Word_occurrence t = *a;
  *a = *b;
  *b = t;
}

// function to find the partition position
int partition(Word_occurrence * array, int low, int high) {
  
  // selezionare l'elemento più a destra come perno
  int pivot = array[high].number_repeats;
  
  // puntatore per l'elemento maggiore
  int i = (low - 1);

  // attraversare ogni elemento dell'array
  // confrontarli con il pivot
  for (int j = low; j < high; j++) {
    if (array[j].number_repeats >= pivot) {
        
      // se viene trovato un elemento più piccolo del pivot
      // lo scambia con l'elemento maggiore indicato da i
      i++;
      
      // scambiare l'elemento in i con l'elemento in j
      swap(&array[i], &array[j]);
    }
  }

  // scambiare l'elemento cardine con l'elemento maggiore in i
  swap(&array[i + 1], &array[high]);
  
  // restituisce il punto di partizione
  return (i + 1);
}

void quickSort(Word_occurrence *array, int low, int high) {
  // Creazione di una pila ausiliaria per simulare la ricorsione
  int stack[high - low + 1];
  
  // Inizializzazione della cima della pila
  int top = -1;
  
  // Push iniziale dei valori di low e high sulla pila
  stack[++top] = low;
  stack[++top] = high;
  
  // Finché la pila non è vuota
  while (top >= 0) {
    // Pop dei valori di low e high dalla pila
    high = stack[top--];
    low = stack[top--];
    
    // Trovare l'elemento pivot
    int pi = partition(array, low, high);
    
    // Se ci sono elementi a sinistra del pivot, push dei loro indici sulla pila
    if (pi - 1 > low) {
      stack[++top] = low;
      stack[++top] = pi - 1;
    }
    
    // Se ci sono elementi a destra del pivot, push dei loro indici sulla pila
    if (pi + 1 < high) {
      stack[++top] = pi + 1;
      stack[++top] = high;
    }
  }
}


void sort_occurrences(Word_occurrence ** p_occurrences, int n){
     
     Word_occurrence * occurrences = *p_occurrences;
     quickSort(occurrences, 0, n - 1);

}