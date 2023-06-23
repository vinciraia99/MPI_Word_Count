#include "wordlist.h"

//#define DEBUG 

int check_if_end_char(char ch){

    if(ch == '\n' || ch =='\r' || ch == '\t'|| ch == ' ' || ch == EOF){
        return 1;

    }
    return 0;
}

void count_word(GHashTable* hash, char * word, int * word_counter){

    gpointer lookup;
    int val;
    lookup = g_hash_table_lookup(hash,word);
    if(lookup == NULL){
        val = 1;
        (*word_counter)++;
    }
    else{
        val = GPOINTER_TO_INT(lookup) + 1;
    }
    g_hash_table_insert(hash,word,GINT_TO_POINTER (val));
}

Word_occurrence* create_inf_to_send(GHashTable* hash, int  *num_occ){

    int length;
    char ** word_list = (char **) g_hash_table_get_keys_as_array (hash , &length);
    char * word;
    gpointer lookup;
    int occ;

    Word_occurrence * occurrences;
    MPI_Alloc_mem(sizeof(Word_occurrence) * length, MPI_INFO_NULL , &occurrences);

    for (int i = 0; i < length; i++)
    {
        word = word_list[i]; 
        lookup = g_hash_table_lookup(hash,word); 
        occ =  GPOINTER_TO_INT(lookup);
        occurrences[i].number_repeats = occ;
        strncpy(occurrences[i].word, word, 46);  
        free(word);
    }

    free(word_list);
    g_hash_table_destroy (hash);
 
    return occurrences;

}

Word_occurrence * get_word_list_from_chunk(Chunk *chunks_received, int chunk_number, int rank, int * num_word){

    FILE *file_to_read;
    
    char word[45];
    int n = 0, lex_num = 0;
    int num_occ = 0;

    Chunk chunk;
    char ch;
    GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
    Word_occurrence * occurrences;

    int flag_word_not_terminate= 0;

    for(int i = 0; i < chunk_number; i++){

        chunk = chunks_received[i];
        file_to_read = fopen(chunk.path , "r");
        #ifdef DEBUG
            printf("\nChunk offset %d: %f\n",rank,chunk.start_offset);
        #endif
        fseek(file_to_read, chunk.start_offset , SEEK_CUR);

        //skip della prima parola se questa non è all'inizio
        if(ftell(file_to_read) != 0){ //ftell ricava la posizione corrente del seek
            fseek(file_to_read,  -1 , SEEK_CUR);
            do{
                ch = fgetc(file_to_read);
            }while(!check_if_end_char(ch));
        }
        
        int state = 0;  

        while (ftell(file_to_read) < (chunk.end_offset)  || flag_word_not_terminate)
        {
            
            word[n] = fgetc(file_to_read);

            switch (state)
            {
                case 0: 
                    if(isalpha(word[n]) || isdigit(word[n])){
                        state = 1;
                        n++;
                        flag_word_not_terminate = 1;   
                    }
                break; 
                case 1: 
                    if(isalpha(word[n]) || isdigit(word[n])){
                        state = 1;
                        n++ ; 
                        flag_word_not_terminate = 1; 
                    }
                    else{
                        if(check_if_end_char(word[n])){
                            word[n] = '\0';
                            for(int j = 0; word[j]; j++){
                                word[j] = tolower(word[j]);
                            }
                            count_word(hash , strdup(word), &lex_num);
                            n = 0;
                            state = 0;
                            flag_word_not_terminate = 0;
                        }  
                    }
                break;
            }
        }

    }

    
    occurrences = create_inf_to_send(hash, &num_occ);
    * num_word = lex_num; 

    return occurrences;


}


