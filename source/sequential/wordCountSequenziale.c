#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_WORD_LENGTH 100
#define MAX_FILENAME_LENGTH 256
#define CSV_OUTPUT_SEQ "word_count_seq.csv"

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
            snprintf(filename, sizeof(filename)+1, "%s/%s", directory, entry->d_name);
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

    fprintf(file, "\"Parola\",\"Frequenza\"\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "\"%s\",\"%d\"\n", wordCounts[i].word, wordCounts[i].count);
    }

    fclose(file);
}

void quicksort(WordCount* wordCounts, int left, int right) {
    if (left >= right) {
        return;
    }

    int pivot = wordCounts[right].count;
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (wordCounts[j].count >= pivot) {
            i++;
            WordCount temp = wordCounts[i];
            wordCounts[i] = wordCounts[j];
            wordCounts[j] = temp;
        }
    }

    WordCount temp = wordCounts[i + 1];
    wordCounts[i + 1] = wordCounts[right];
    wordCounts[right] = temp;

    int partition = i + 1;

    quicksort(wordCounts, left, partition - 1);
    quicksort(wordCounts, partition + 1, right);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Nessuna cartella passata come argomento\n");
        return 1;
    }else{
        printf("Utilizzo la cartella: %s\n", argv[1]);
    }

    char *directory = argv[1];

    WordCount wordCounts[1000];
    int count = 0;

    processDirectory(directory, wordCounts, &count);

    quicksort(wordCounts, 0, count - 1);

    char outputFilename[MAX_FILENAME_LENGTH];
    snprintf(outputFilename, sizeof(outputFilename), "%s", CSV_OUTPUT_SEQ);

    writeCSV(outputFilename, wordCounts, count);

    printf("Word count completato! Risultati salvati in %s\n", outputFilename);

    return 0;
}
