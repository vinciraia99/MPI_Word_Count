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

### Step 1: Come vengono divisi i file

