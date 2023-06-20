#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

typedef struct {
    char parola[MAX_LEN];
    int frequenza;
} RigaCSV;

int confrontaCSV(const char* nomeFile1, const char* nomeFile2) {
    FILE *file1, *file2;
    char riga1[MAX_LEN], riga2[MAX_LEN];
    RigaCSV csv1[MAX_LEN], csv2[MAX_LEN];
    int numRighe1 = 0, numRighe2 = 0;

    file1 = fopen(nomeFile1, "r");
    file2 = fopen(nomeFile2, "r");

    if (file1 == NULL || file2 == NULL) {
        printf("Impossibile aprire i file.\n");
        return 0;
    }

    // Ignora la prima riga
    fgets(riga1, MAX_LEN, file1);
    fgets(riga2, MAX_LEN, file2);

    // Lettura del primo file CSV
    while (fgets(riga1, MAX_LEN, file1) != NULL) {
        sscanf(riga1, "\"%[^\"]\",\"%d\"", csv1[numRighe1].parola, &csv1[numRighe1].frequenza);
        numRighe1++;
    }

    // Lettura del secondo file CSV
    while (fgets(riga2, MAX_LEN, file2) != NULL) {
        sscanf(riga2, "\"%[^\"]\",\"%d\"", csv2[numRighe2].parola, &csv2[numRighe2].frequenza);
        numRighe2++;
    }

    fclose(file1);
    fclose(file2);

    // Controllo se i due file hanno lo stesso numero di righe
    if (numRighe1 != numRighe2)
        return 0; // I file hanno lunghezza diversa

    // Controllo se ogni riga del primo file esiste anche nel secondo file
    for (int i = 0; i < numRighe1; i++) {
        int trovato = 0;

        for (int j = 0; j < numRighe2; j++) {
            if (strcmp(csv1[i].parola, csv2[j].parola) == 0 && csv1[i].frequenza == csv2[j].frequenza) {
                trovato = 1;
                break;
            }
        }

        if (!trovato) {
            printf("Elemento differente:\n");
            printf("Parola: %s\n", csv1[i].parola);
            printf("Occorrenza: %d\n", csv1[i].frequenza);
            return 0; // I file sono diversi
        }
    }

    return 1; // I file sono uguali
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Utilizzo: confronta_csv <file1.csv> <file2.csv>\n");
        return 1;
    }

    const char* nomeFile1 = argv[1];
    const char* nomeFile2 = argv[2];

    int risultato = confrontaCSV(nomeFile1, nomeFile2);

    if (risultato)
        printf("I file sono uguali.\n");

    return 0;
}
