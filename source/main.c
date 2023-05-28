#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <math.h>
#include <unistd.h>

#include "mpi.h"
#include "file_info.h"
#include "log.h"
#include "lexer.h"

#define COMM_TAG 88
#define MASTER_RANK 0
#define EXIT_CODE_NO_DIR 2
#define EXIT_CODE_NO_PROC 3
#define EXIT_CODE_DIR_NOT_EXIST 4
#define CSV_OUTPUT "word_count.csv"
#define DEBUG

int main (int argc, char *argv[]){

    int numproc;
    int rank;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status Stat;

    //Creazione del datatype per il chunk 
    MPI_Datatype chunktype;
    create_chunk_datatype(&chunktype);

    MPI_Datatype wordtype;
    create_word_datatype(&wordtype);

    // controllo che il numero di processi sia almeno maggiore o uguale a due 
    // se non ci sono più di due processi non ha senso parallelizzare
    if(argc<2){
        MPI_Abort(MPI_COMM_WORLD, EXIT_CODE_NO_DIR);
        MPI_Finalize();
        return 0;
    }
    if (numproc < 2){ 
        MPI_Abort(MPI_COMM_WORLD, EXIT_CODE_NO_PROC);
        MPI_Finalize();
        return 0;
    }
    //Master
    if(MASTER_RANK == rank){
        
        //Ottengo informazioni sui file all'interno della cartella (dimensione e nome)
        double total_size = 0;
        GList * files;
        //se la cartella in input non esiste non si può continuare.
        files = get_files_info_from_dir(argv[1], &total_size);
        if(files == NULL){
            MPI_Abort(MPI_COMM_WORLD, EXIT_CODE_DIR_NOT_EXIST);
            MPI_Finalize();
            return 0;
        }
        int temp = total_size / (numproc-1);
        double partition_size = temp;
        int remnant = (int) total_size % (numproc-1);
    
        //Produzione dei chunk per ogni processo che non sia quello master
        File_info * currentFileInfo;
        GList * current_item = files;
        int file_nums = g_list_length(files);
        #ifdef DEBUG
            printf("\nNumero dei file: %d\n",file_nums);
        #endif

        //La size dei chunk per ogni processo può essere al più uguale al numero dei file
        //Equivalente senza MPI : Chunk ** chunks_to_send = malloc(sizeof(Chunk *) * numproc -1);
        //Creo una matrice di chunk, un array per ogni processo
        Chunk ** chunks_to_send;
        MPI_Alloc_mem(sizeof(Chunk *) * (numproc -1), MPI_INFO_NULL , &chunks_to_send);

        double file_offset, to_assign = 0;
        double remaining_file;
        int task_to_assign = 0, chunk_count = 0;
        int distr_remnant = 1;

        int pack_size;
    
        //Istanzio array di request e status per l'invio e la ricezione asincrona
        MPI_Request* requests;
        MPI_Alloc_mem(sizeof(MPI_Request) * (numproc -1), MPI_INFO_NULL , &requests);

        for(int i=0; i<file_nums; i++){

            //Prendo file
            currentFileInfo = (File_info *) current_item->data;
            file_offset = 0;
            remaining_file = currentFileInfo->size;

            while(remaining_file > 0){
                if(to_assign == 0){
                    //se chunk_count non è zero posso inviare 
                    if(chunk_count!=0){
                        MPI_Isend(chunks_to_send[task_to_assign - 1], chunk_count, chunktype, task_to_assign, COMM_TAG, MPI_COMM_WORLD, &(requests[task_to_assign-1]));
                    }  
                    to_assign = partition_size;
                    task_to_assign++; 
                    MPI_Alloc_mem(sizeof(Chunk)* file_nums, MPI_INFO_NULL , &(chunks_to_send[task_to_assign - 1]));
                    chunk_count = 0;
                }

                if(distr_remnant && task_to_assign == numproc -1){
                    to_assign = partition_size + remnant;
                    distr_remnant = 0;
                }

                //se la grandezza del file è più piccolo o uguale di quello da assgenare lo assegno tutto
                if(remaining_file <= to_assign){
                    chunks_to_send[task_to_assign - 1][chunk_count].start_offset = file_offset;
                    chunks_to_send[task_to_assign - 1][chunk_count].end_offset = file_offset + remaining_file;
                    strncpy(chunks_to_send[task_to_assign - 1][chunk_count].path , currentFileInfo->path, 260);
                    chunk_count++;
                    #ifdef DEBUG
                        printf("Task to assign: %d \t",task_to_assign - 1);
                        print_file_chunk(chunks_to_send[task_to_assign - 1]);
                    #endif
                    to_assign -= remaining_file;
                    remaining_file = 0;
                }
                else{ // altrimenti devo assegnare solo una porzione
                    chunks_to_send[task_to_assign - 1][chunk_count].start_offset = file_offset;
                    chunks_to_send[task_to_assign - 1][chunk_count].end_offset = file_offset + to_assign;
                    strncpy(chunks_to_send[task_to_assign - 1][chunk_count].path , currentFileInfo->path, 260);
                    chunk_count++;
                    #ifdef DEBUG
                        printf("Task to assign: %d \t",task_to_assign - 1);
                        print_file_chunk(chunks_to_send[task_to_assign - 1]);
                    #endif
                    file_offset += to_assign;
                    remaining_file -= to_assign;
                    to_assign = 0;
                }   
            }

            current_item = current_item->next;

        }

        #ifdef DEBUG
            printf("Last Task to assign: %d \t",task_to_assign - 1);
            print_file_chunk(chunks_to_send[task_to_assign - 1]);
        #endif
         
        MPI_Isend(chunks_to_send[task_to_assign - 1], chunk_count, chunktype, task_to_assign, COMM_TAG, MPI_COMM_WORLD, &(requests[task_to_assign-1]));

        MPI_Waitall(numproc - 1, requests , MPI_STATUSES_IGNORE);

        //Mentre i worker lavorano il master può deallocare tutto ciò che non serve
        MPI_Free_mem(requests);
        for(int i = 0; i< numproc -1; i++){
            MPI_Free_mem(chunks_to_send[i]);
        }
        MPI_Free_mem(chunks_to_send);
        free_files_info(files, file_nums);

        //Ricevere da tutti i figli
        MPI_Status stat;
        int words_in_message, length, occ;
        Word_occurrence * occurrences;
        GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
        gpointer lookup;
    
        for(int i = 0; i < numproc -1; i++){
            MPI_Probe(MPI_ANY_SOURCE, COMM_TAG, MPI_COMM_WORLD, &stat);
            MPI_Get_count(&stat, wordtype, &words_in_message);
            #ifdef DEBUG
                printf("Word in message on rank %d: %d \n",i,words_in_message);
            #endif
            MPI_Alloc_mem(sizeof(Word_occurrence ) * words_in_message, MPI_INFO_NULL , &occurrences);
            MPI_Recv(occurrences, words_in_message, wordtype, stat.MPI_SOURCE, COMM_TAG, MPI_COMM_WORLD, &stat);
            for(int j=0; j< words_in_message; j++){
                lookup = g_hash_table_lookup(hash,occurrences[j].parola);
                if(lookup == NULL){
                    g_hash_table_insert(hash,occurrences[j].parola,GINT_TO_POINTER (occurrences[j].parola));
                }
                else{
                    g_hash_table_insert(hash,occurrences[j].parola,GINT_TO_POINTER (occurrences[j].parola + GPOINTER_TO_INT(lookup)));
                }
            } 
        }

        char ** entries = (char **) g_hash_table_get_keys_as_array (hash , &length);
        FILE *out = fopen (CSV_OUTPUT , "w");
        Word_occurrence * to_order = malloc(sizeof(Word_occurrence) * length);
        for(int i = 0; i<length; i++){

            lookup = g_hash_table_lookup(hash,entries[i]); 
            occ =  GPOINTER_TO_INT(lookup);
            to_order[i].numero_ripetizioni = occ;
            strncpy(to_order[i].parola, entries[i], sizeof(to_order[i].parola));  
        }

        sort_occurrences(&to_order, length);

        fprintf(out, "\"Parola\",\"Frequenza\"\n");
        for(int i = 0; i< length; i++){
            fprintf(out,"\"%s\",\"%d\"\n", to_order[i].parola, to_order[i].numero_ripetizioni);
        }

        MPI_Free_mem(occurrences);
        free(to_order);
        free(entries);
    }
    else{

        Chunk * chunk_to_recv;
        int chunk_number = 0, num_occ = 0;
        Word_occurrence * occurrences;
        chunk_to_recv = probe_recv_chunks(chunktype, Stat, &chunk_number);

        occurrences = get_lexeme_from_chunk(chunk_to_recv, chunk_number, rank, &num_occ);
        MPI_Send(occurrences, num_occ, wordtype, 0, COMM_TAG, MPI_COMM_WORLD);

        MPI_Free_mem(chunk_to_recv);
        MPI_Free_mem(occurrences);
              
    }

    MPI_Type_free(&chunktype);
    MPI_Type_free(&wordtype);
    MPI_Finalize();

    return 0;

}