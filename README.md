# MPI_Word_Count
>**MPI_Word_Count** è un'implementazione parallela di word count per il conteggio delle parole. Il word count viene utilizzato per diverse applicazioni come limiti di parole, analisi dei dati e indicizzazione dei documenti.

## Indice dei contenuti
1. [Descrzione del problema](#descrzione-del-problema)
2. [Soluzione proposta](#soluzione-proposta)
3. [Struttura del progetto](#struttura-del-progetto)
4. [Input/Output del programma](#inputoutput-del-programma)


# Descrzione del problema
Il problema in questione può essere risolto prendendo spunto da map-reduce e riscrivednolo utilizzando MPI. 

Per eseguire velocemente il conteggio delle parole su un gran numero di file l'ideale è:

- Il **MASTER** deve leggere tutti i file all'interno di una directory. 
- Gli altri processi riceve la propria porzione sul totale di tutti i file inviata dal processo **MASTER**. 
- Ogni processo **SLAVE** che ha ricevuto la propria porzione dovrà effettuare su di essa un conteggio delle parole, per tenere traccia della frequenza di ogni parola trovata.
- Tutti i processi **SLAVE** salveranno una struttura formata da ["parola","frequenza"] in locale
- La parole elaborate cone le frequenze vengono inviate da ogni processo al **MASTER**, che dovrà effettuare un merge. 
- Infine, il **MASTER** produrrà un file CSV contenente le frequenze ordinate.

# Soluzione proposta
Per risolvere l'aspetto rigaurdante quello di dividere in maniera equa la computazione tra i processi,l'algoritmo opera come segue:

1. Il processo **MASTER** accetta una cartella come input da linea di comando, contenente i file su cui eseguire il conteggio delle parole. I file vengono suddivisi in modo equo, considerando la loro dimensione in byte, evitando di scorrerli per la divisione. Infine esso invia ad ogni processo riceve una o più partizioni di file da elaborare.
<br><br>Ogni partizione è formata da:
    - Un campo che **path** che contiene il path del file a cui è associata la partizione
    - Un campo **start_offset** indica il byte all'interno del file da cui la partizione inizia.
    - Il campo **end_offset** indica il byte all'interno del file in cui la partizione finisce.

> È possibile vedere un'implementanzione della struttura nel file [mpi_chunk.c](/source/libraries/mpi_chunk.h)

La somma delle dimensioni delle partizioni ricevute sarà uguale per ogni processo. 

2. I processi **SLAVE** ricevono la propria porzione di file e iniziano a conteggiare le parole, calcolandone la frequenza.

3. Le frequenze calcolate vengono inviate dagli **SLAVE** al **MASTER**.

4. Il **MASTER** riceve le frequenze da tutti gli altri processi e le combina. Infine, produce un file CSV contenente il conteggio preciso delle parole.

Questo è un esempio di file CSV di output:

<img src="img/output_csv_computazione.png" alt="output csv computazione" width = "30%" height = "50%">

## Struttura del progetto
<img src="img/struttura_del_progetto.png" alt="struttura del progetto" width = "40%" height = "50%">

- **img** contiene tutte le immagini per la documentazione.
- **source** contiene tutte i file .c, il makefile e uno script chiamato [install_dependency_and_compile.sh](./source/install_dependency_and_compile.sh) che scarica e installa tutte le dipendenze nel necessarie al funzionamento del codice ed effettua una make clean all per compilare il codice dando in output word_count.out
    - **libraries** contiene tutti gli header utilizzati per il progetto
        - **implementation** contiene tutte le implementazione degli header
- **file_di_testo** contiene parole di prova per provare il programma
- **benchmark_script** contiene due script per eseguire il benchmark per la weak e la strong scalability 
- **correctness_script** contiene uno script che verifica la correttezza del codice.
- **utilities** contiene uno script per la generazione di file di testo casuali utili per eseguire il benchmark di weak e strong scalability

## Input/Output del programma
### Input
Il programma prende in input passato come argomento il path di una direcory contenente un serie di file di testo
### Output
Il programma fornisce in output un file CSV chiamato "word_count.csv" contenente una lista di parole e la frequenza (cioè il nunero di volte che sono state trovate all'interno dei vari file).

## Implementazione

### Possibili eccezioni 
Il programma verifica tre condizioni necessarie per proseguire e fornisce in base all'errore un "exit status" diverso per ogni errore:

- Se manca l'argormento al lancio del programma, quindi di fatto non è stato dichiarata la cartella di input viene restuito un **exit status corrispondente a 2**
- Se i processi che lanciano il programma sono <2 viene restuito un **exit status corrispondente a 3**
- Se la directory data come argomento non esiste realmente viene restituito un **exit status corrispondente a 4**

### Step 1: Come vengono divisi i file e salvati
I file presi in input vengono divisi dal MASTER in partizioni denominate Chunk. Con i file presi in input viene ricava dimensione in byte di ogni file utilizzando l'istruzione: 

```
File_info * get_file_info_from_path(char * path){

    struct stat sb;
    File_info * file = (File_info *) malloc(sizeof(File_info));

    if(stat(path, &sb) != -1){
        file->size = sb.st_size;
        file->path = malloc(sizeof(char) * strlen(path) + 1);
        strncpy(file->path ,path , strlen(path)+1);
    }

    return file;
    
}
```
>L'istruzione è presente nel file [file_info.c](source/libraries/implementation/file_info.c)

Dopodichè somma la dimensione di ogni file per ottenere la taglia totale dell'input da dividere tra i processi.

```
double partition_size = (int)  total_size / (numproc-1);
```
>L'istruzione è presente nel file [main.c](source/main.c)

Ora il programma deve distribuire i file ai processi SLAVE. Per farlo divide il totale per ogni processo e se la taglia totale non è un multiplo deli numeri di SLAVE si assegnano i byte in "eccesso" all'ultimo processo. 

Dopo aver diviso i vari blocchi da assegnare ad ogni processo il MASTER scrive lo start offset, l'end offset e il path del file all'interno di una struct:

```
typedef struct Chunk{
    double start_offset;
    double end_offset;
    char path[300];
} Chunk;
```
>L'istruzione è presente nel file [main.c](source/main.c)

Una volta calcolati li carica in un array "`chunks_array`" 
### Step 2: Come vengono inviati i chunk creati
Ebbene notare che il processo MASTER per non perdere tempo non calcola i chunk e poi li invia ma li invia non appena un chunk viene prodotto ai processi SLAVE in ordine di rank.

Per non rimanere in attesa l'invio dei chunk avviene in maniera asincrona con la funzione MPI_Isend. In questo modo il MASTER può continuare il calcolo dei chunk per i restanti processi senza fermarsi.

```
MPI_Isend(chunks_array[task_to_assign - 1], chunk_count, chunktype, task_to_assign, COMM_TAG, MPI_COMM_WORLD, &(requests[task_to_assign-1]));
```
>L'istruzione è presente nel file [main.c](source/main.c)

**La documentazione raccomanda** di non modificare il buffer inviato finchè non si è sicuri che l'operazione di invio sia stata completata, per tale motivo, ogni **SLAVE** viene allocato un apposito buffer.

```
MPI_Probe(MASTER_RANK, COMM_TAG, MPI_COMM_WORLD, &Stat);  

MPI_Get_count(&Stat, chunktype, &chunk_number); 

MPI_Alloc_mem(sizeof(Chunk ) * (chunk_number), MPI_INFO_NULL , &chunk_to_recv);

MPI_Recv(chunk_to_recv, chunk_number, chunktype, MASTER_RANK, COMM_TAG, MPI_COMM_WORLD, &Stat); 
```

Una volta inviati i chunk, il **MASTER** attende che tutte le operazioni di invio siano completate utilizzando MPI_Waitall. 
```
MPI_Waitall(numtasks - 1, reqs , MPI_STATUSES_IGNORE);
```

Infine dealloca tutti buffe che sono stati allocatri dinamicamente.

```
 for(int i = 0; i< numproc -1; i++){
            MPI_Free_mem(chunks_array[i]);
        }
MPI_Free_mem(chunks_array);
free_files_info(files, file_nums);
```
`free_files_info` è una funzione che svuota la lista di file

```
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
```

