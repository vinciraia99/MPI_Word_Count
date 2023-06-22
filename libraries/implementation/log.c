#include <stdio.h>
#include "file_info.h"
#include "log.h"

//Questa funzione stampa il nome del file e la dimensione in byte
void print_file_info(File_info *file){
    printf("Name of file: %s\n" , file->path);
    printf("Dimension of file: %ld byte\n\n", file->size);
}

// Questa funzione stampa le informazioni di un chunk 
void print_file_chunk(Chunk * chunk){
    printf("Start offset: %f\n" , chunk->start_offset);
    printf("End offset: %f\n" , chunk->end_offset);
    printf("Path: %s\n\n" , chunk->path);
}

// Questa funzione stampa un array di chunk 
void print_file_chunk_array(Chunk * chunks, int num_elements){
    for (int i = 0; i< num_elements; i++){
        printf("Start offset: %f\n" , chunks[i].start_offset);
        printf("End offset: %f\n" , chunks[i].end_offset);
        printf("Path: %s\n\n" , chunks[i].path);
    }

}