#include <stdio.h>
#include "file_info.h"
#include "log.h"

//Questa funzione stampa il nome del file e la dimensione in byte
void print_file_info(File_info *file){
    printf("Nome del file: %s\n" , file->path);
    printf("Dimensione del file: %ld byte\n\n", file->size);
}

void print_file_chunk(Chunk * chunk){
    printf("start_offset: %f\n" , chunk->start_offset);
    printf("end_offset: %f\n" , chunk->end_offset);
    printf("path: %s\n\n" , chunk->path);
}

void print_file_chunk_array(Chunk * chunks, int num_elements){
    for (int i = 0; i< num_elements; i++){
        printn_file_chunk(chunks[i]);
    }

}

void print_received_chunk(Chunk * chunk, int num_elements, int rank){

    char file_output[50];
    char rankc[2];
    rankc[0] = rank + '0';
    rankc[1] = '\0';
    strncpy(file_output, "out", 50);
    strncat(file_output, rankc, 50);
    FILE * out = fopen (file_output , "w");

    for (int i = 0; i< num_elements; i++){
        fprintf(out,"[Worker  %d]\n", rank);
        fprintf(out,"start_offset %f\n" , chunk[i].start_offset);
        fprintf(out,"end_offset %f\n" , chunk[i].end_offset);
        fprintf(out,"path %s\n\n" , chunk[i].path);
    }

}