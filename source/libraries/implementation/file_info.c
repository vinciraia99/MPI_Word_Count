#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <glib.h>
#include <errno.h>
#include "file_info.h"
#include "log.h"

//#define DEBUG 

//Riceve in input un percorso e restuisce la struttura file_info contenente la dimensione del file espressa in byte e il percorso del file espresso come stringa
File_info * get_file_info_from_path(char * path){

    struct stat sb;
    File_info * file = (File_info *) malloc(sizeof(File_info));

    if(stat(path, &sb) != -1){
        file->size = sb.st_size;
        file->path = malloc(sizeof(char) * strlen(path) + 1);
        strncpy(file->path ,path , strlen(path)+1);
        #ifdef DEBUG
            printf("Percorso:%s \n",path);
            printf("Dimensione file:%ld byte \n",sb.st_size);
        #endif
    }

    return file;
    
}

//Funzione che crea la lista di oggetti file_info e riempie la variabile files_size con il totale espresso in byte partendo da una cartella in input
GList * get_files_info_from_dir(char * dir_paths, double *files_size){

    GList* list = NULL;
    DIR *directory;
    struct dirent *dir;
    char file_path[260];
    char path_separator[]={'/','\0'};
    char end[] ={'\0'};

    directory = opendir(dir_paths);

    if (directory)
    {
        File_info * temp_info;
        while ((dir = readdir(directory)) != NULL)
        {
            if(dir-> d_type != DT_DIR){
                strncpy(file_path, dir_paths, 260);
                strncat(file_path, path_separator, 2);
                strncat(file_path, dir->d_name, strlen(dir->d_name));
                
                #ifdef DEBUG
                    printf("Nome file:%s \n",file_path);
                #endif
            
                temp_info = get_file_info_from_path(file_path);
                *files_size += temp_info->size;
                
                list = g_list_append(list, temp_info);

            }

        }

        closedir(directory);
        
    }
    else if (ENOENT == errno){
     return NULL;
    }

    #ifdef DEBUG
        printf("Dimensione totale:%f byte \n",*files_size);
    #endif

    return list;

}

void free_files_info(GList * list, int num){

    GList * elem = list;
    for(int i=0; i<num; i++){
            File_info * f = elem->data;
            #ifdef DEBUG
                printf("File che viene cancellato:\n");
                print_file_info(f);
            #endif
            free(f->path);
            free(elem->data);
            elem = elem->next;
    }
    g_list_free(list);
}

