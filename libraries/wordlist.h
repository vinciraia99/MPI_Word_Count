#include <stdio.h>
#include <ctype.h>
#include <glib.h>

#include "mpi_chunk.h"
#include "word.h"

Word_occurrence * get_word_list_from_chunk(Chunk *chunks_received, int chunk_number, int rank, int * num_word);