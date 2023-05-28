#include <stdio.h>
#include<ctype.h>
#include <glib.h>

#include "chunk.h"
#include "word.h"

Word_occurrence * get_lexeme_from_chunk(Chunk *chunks_received, int chunk_number, int rank, int * num_word);