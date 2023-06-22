#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_WORD_LENGTH 20
#define WORDS_PER_LINE 10 // Numero di parole per riga

char* getRandomWord() {
    char* words[] = {
        "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur",
        "adipiscing", "elit", "sed", "do", "eiusmod", "tempor",
        "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua."
    };

    int numWords = sizeof(words) / sizeof(words[0]);
    int randomIndex = rand() % numWords;
    return words[randomIndex];
}

void createRandomTextFile(int fileSizeMB, const char* fileName) {
    long fileSize = fileSizeMB * 1024 * 1024; // Conversione da MB a byte
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Impossibile creare il file.\n");
        return;
    }

    srand(time(NULL)); // Inizializza il generatore di numeri casuali

    long bytesWritten = 0;
    int wordsWritten = 0;
    char word[MAX_WORD_LENGTH + 1];

    while (bytesWritten < fileSize) {
        strcpy(word, getRandomWord());
        int wordLength = strlen(word);

        if (bytesWritten + wordLength + 1 <= fileSize) {
            fprintf(file, "%s ", word);
            bytesWritten += wordLength + 1;
            wordsWritten++;

            // Aggiungi un accapo ogni tot parole
            if (wordsWritten % WORDS_PER_LINE == 0) {
                fprintf(file, "\n");
            }
        } else {
            break;
        }
    }

    fclose(file);
    printf("File creato con successo: %s\n", fileName);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Utilizzo: %s <dimensione_file_MB> <nome_file>\n", argv[0]);
        return 1;
    }

    int fileSize = atoi(argv[1]);
    if (fileSize <= 0) {
        printf("La dimensione del file deve essere un numero positivo.\n");
        return 1;
    }

    const char* fileName = argv[2];
    createRandomTextFile(fileSize, fileName);

    return 0;
}
