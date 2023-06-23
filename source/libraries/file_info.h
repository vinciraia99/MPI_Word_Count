#include <glib.h>
#include "mpi.h"

typedef struct File_info{

    long int size;
    char * path;

} File_info;

GList * get_files_info_from_dir(char * dir_paths, double *files_size);

void free_files_info(GList * list, int num);

