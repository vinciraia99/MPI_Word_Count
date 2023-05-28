#include "chunk.h"

void print_file_info(File_info *file);

void print_file_chunk(Chunk * chunk);

void printn_file_chunk(Chunk chunk);

void print_file_chunk_array(Chunk * chunks, int num_elements);

void print_received_chunks(Chunk * chunks, int num_elements, int rank);