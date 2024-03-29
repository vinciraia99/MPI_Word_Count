#include <stdlib.h>
#include <string.h>

#include "mpi.h"

#define MAX_PACK_SIZE 8192
#define COMM_TAG 10
#define MASTER_RANK 0

#ifndef CHUNK
#define CHUNK
    typedef struct Chunk{
        double start_offset;
        double end_offset;
        char path[300];
    } Chunk;
#endif

Chunk* create_chunk(double start_offset , double end_offset, char * path);

void create_chunk_datatype(MPI_Datatype *chunktype);