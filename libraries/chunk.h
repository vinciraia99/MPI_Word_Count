#include <stdlib.h>
#include <string.h>

#include "mpi.h"

#define MAX_PACK_SIZE 8192
#define COMM_TAG 88
#define MASTER_RANK 0

#ifndef CHUNK
#define CHUNK
    typedef struct Chunk{

        double start_offset;
        double end_offset;
        char path[260];

    } Chunk;
#endif

Chunk* create_file_chunk(double start_offset , double end_offset, char * path);

void create_chunk_datatype(MPI_Datatype *chunktype);

Chunk * probe_recv_chunks(MPI_Datatype chunktype, MPI_Status Stat, int * chunk_number);