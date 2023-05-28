#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_WORD_LENGTH 100
#define MAX_FILENAME_LENGTH 256

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

WordCount* findWord(WordCount* wordCounts, int count, char* word) {
    for (int i = 0; i < count; i++) {
        if (strcmp(wordCounts[i].word, word) == 0) {
            return &wordCounts[i];
        }
    }
    return NULL;
}

void processFile(char* filename, WordCount* wordCounts, int* count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Impossibile aprire il file: %s\n", filename);
        return;
    }

    char word[MAX_WORD_LENGTH];

    while (fscanf(file, "%s", word) == 1) {
        WordCount* foundWord = findWord(wordCounts, *count, word);
        if (foundWord != NULL) {
            foundWord->count++;
        } else {
            WordCount newWord;
            strcpy(newWord.word, word);
            newWord.count = 1;
            wordCounts[*count] = newWord;
            (*count)++;
        }
    }

    fclose(file);
}

void processDirectory(char* directory, WordCount* wordCounts, int* count) {
    DIR* dir = opendir(directory);
    if (dir == NULL) {
        printf("Impossibile aprire la cartella: %s\n", directory);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Considera solo file regolari
            char filename[MAX_FILENAME_LENGTH];
            snprintf(filename, sizeof(filename), "%s/%s", directory, entry->d_name);
            processFile(filename, wordCounts, count);
        }
    }

    closedir(dir);
}

void writeCSV(char* filename, WordCount* wordCounts, int count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Impossibile creare il file: %s\n", filename);
        return;
    }

    fprintf(file, "Parola,Occorrenze\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%d\n", wordCounts[i].word, wordCounts[i].count);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Utilizzo: %s <cartella>\n", argv[0]);
        return 1;
    }

    char *directory = argv[1];

    WordCount wordCounts[1000];
    int count = 0;

    processDirectory(directory, wordCounts, &count);

    char outputFilename[MAX_FILENAME_LENGTH];
    snprintf(outputFilename, sizeof(outputFilename), "%s/word_count.csv", directory);

    writeCSV(outputFilename, wordCounts, count);

    printf("Word count completato! Risultati salvati in %s\n", outputFilename);

    return 0;
}
