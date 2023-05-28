#include "chunk.h"  
#include <stdio.h> 

#define DEBUG 

// Funzione per creare un'istanza di Chunk
Chunk* create_file_chunk(double start_offset , double end_offset, char * path){
    Chunk * new_chunk = malloc(sizeof(Chunk));  // Alloca la memoria per la struttura Chunk
    new_chunk->start_offset = start_offset;  // Imposta l'offset di inizio
    new_chunk->end_offset = end_offset;  // Imposta l'offset di fine
    strncpy(new_chunk->path, path, 260);  // Copia il percorso nel campo path
    return new_chunk;  // Restituisce il puntatore alla struttura Chunk creata
}

// Funzione per creare il tipo di dato MPI personalizzato per Chunk
void create_chunk_datatype(MPI_Datatype *chunk_type){
    MPI_Datatype types[2];  // Array di tipi di dati di base
    int block_counts[2];  // Array di conteggi dei blocchi

    MPI_Aint offsets[2], lb, extent;  // Array di offset e variabili per il calcolo degli estremi

    offsets[0] = 0;
    types[0] = MPI_DOUBLE;
    block_counts[0] = 2;

    MPI_Type_get_extent(MPI_DOUBLE, &lb, &extent);  // Ottiene l'estensione del tipo di dato MPI_DOUBLE
    offsets[1] = 2 * extent;
    types[1] = MPI_CHAR;
    block_counts[1] = 260;

    MPI_Type_create_struct(2, block_counts, offsets, types, chunk_type);  // Crea il tipo di dato strutturato personalizzato
    MPI_Type_commit(chunk_type);  // Conferma il tipo di dato personalizzato
    #ifdef DEBUG
        printf("\nCREA CHUNK DATATYPE\n");
    #endif
}

// Funzione per ricevere i chunk tramite MPI
Chunk * probe_recv_chunks(MPI_Datatype chunk_type, MPI_Status status, int * num_chunks){
    *num_chunks = 0;
    MPI_Probe(MASTER_RANK, COMM_TAG, MPI_COMM_WORLD, &status);  // Probe per verificare la ricezione dei messaggi da parte del master
    MPI_Get_count(&status, chunk_type, num_chunks);  // Ottiene il numero di chunk da ricevere
    Chunk * chunks_to_recv = NULL;
    MPI_Alloc_mem(sizeof(Chunk ) * (*num_chunks), MPI_INFO_NULL , &chunks_to_recv);  // Alloca la memoria per i chunk da ricevere
    MPI_Recv(chunks_to_recv, *num_chunks, chunk_type, MASTER_RANK, COMM_TAG, MPI_COMM_WORLD, &status);  // Riceve i chunk
    return chunks_to_recv;  // Restituisce i chunk ricevuti
}
