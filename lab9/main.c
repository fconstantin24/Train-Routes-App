#include <stdio.h>
#include <stdlib.h>

#define INFINIT 1000000

/* Functie care elimina \n de la fgets */
void strip(char *string) {
    string[strlen(string) - 1] = '\0';
}

/* Functie care cauta indexul unui oras din lista de orase */
int cauta_oras(char **orase, int nr_orase, char *oras) {
    for (int i = 0; i < nr_orase; i++) {
        if (strcmp(orase[i], oras) == 0) {
            return i;
        }
    }
    return -1;
}

/* Functie pentru a gasi statiile consecutive cu distanta minima/maxima intre ele */
void statii_min_max(int **adiacenta, int **distante, char **orase, int nr_orase) {

    /* Initializam cu -1 valorile */
    int minim_start = -1;
    int minim_end = -1;
    int minim_val = -1;

    int maxim_start = -1;
    int maxim_end = -1;
    int maxim_val = -1;

    /* Parcurgem muchiile din graf*/
    for (int i = 0; i < nr_orase; i++) {
        for (int j = 0; j < nr_orase; j++) {
            if (adiacenta[i][j] == 1) {

                /* Daca variabilele sunt -1, se actualizeaza toate */
                if (minim_start == -1) {
                    minim_val = distante[i][j];
                    minim_start = i;
                    minim_end = j;

                    maxim_val = distante[i][j];
                    maxim_start = i;
                    maxim_end = j;
                } else {
                    if (distante[i][j] > maxim_val) {  // Se actualizeaza maximul
                        maxim_val = distante[i][j];
                        maxim_start = i;
                        maxim_end = j;
                    } else if (distante[i][j] < minim_val) {  // Se actualizeaza minimul
                        minim_val = distante[i][j];
                        minim_start = i;
                        minim_end = j;
                    }
                }
            }
        }
    }

    /* Se afiseaza in fisier minimul */
    FILE *output_min = fopen("min.txt", "w");
    fprintf(output_min, "%s - %s : %d km\n", orase[minim_start], orase[minim_end], minim_val);
    fclose(output_min);

    /* Se afiseaza in fisier maximul */
    FILE *output_max = fopen("max.txt", "w");
    fprintf(output_max, "%s - %s : %d km\n", orase[maxim_start], orase[maxim_end], maxim_val);
    fclose(output_max);
}

/* Functie care calculeaza numarul de km de cale ferata care exista in baza de date */
void distanta_cale_ferata(int **adiacenta, int **distante, int nr_orase) {
    /* Initializam distanta cu 0 */
    int distanta = 0;

    /* Parcurgem fiecare muchie din graf */
    for (int i = 0; i < nr_orase; i++) {
        for (int j = i + 1; j < nr_orase; j++) {
            if (adiacenta[i][j] == 1) {
                /* Actualizam distanta */
                distanta += distante[i][j];
            }
        }
    }

    /* Scriem distanta in fisier */
    FILE *output = fopen("total.txt", "w");
    fprintf(output, "Cale ferata in baza de date: %d km\n", distanta);
    fclose(output);
}

/* Functie pentru afisarea unui drum din rezultatul algoritmului Floyd-Warshall */
void afisare_drum(int **next, char **orase, int u, int v, FILE *output) {
    if (u == v) {
        fprintf(output, "%s\n", orase[v]);
        return;
    }
    fprintf(output, "%s, ", orase[u]);
    afisare_drum(next, orase, next[u][v], v, output);
}

/* Functie pentru subpunctele 5, 6, 8 */
void dist_braila_departe(int **adiacenta, int **distante, char **orase, int nr_orase) {

    /* Alocam matricea dist pentru algoritm */
    int **dist = malloc(1000 * sizeof(int*));
    for (int i = 0; i < 1000; i++) {
        dist[i] = malloc(1000 * sizeof(int));
        for (int j = 0; j < 1000; j++) {
            dist[i][j] = INFINIT;  // Initializam cu infinit
        }
    }

    /* Alocam matricea next pentru algoritm */
    int **next = malloc(1000 * sizeof(int*));
    for (int i = 0; i < 1000; i++) {
        next[i] = malloc(1000 * sizeof(int));
    }

    for (int i = 0; i < nr_orase; i++) {
        dist[i][i] = 0;
    }

    /* Pas k = 0 */
    for (int u = 0; u < nr_orase; u++) {
        for (int v = 0; v < nr_orase; v++) {
            if (u != v && adiacenta[u][v] == 1) {
                dist[u][v] = distante[u][v];
                next[u][v] = v;
            }
        }
    }

    for (int k = 0; k < nr_orase; k++) {
        for (int i = 0; i < nr_orase; i++) {
            for (int j = 0; j < nr_orase; j++) {
                if (k != i && k != j && i != j) {
                    /* Pentru fiecare nod intermediar posibil, calculam distanta */
                    int cost_nou = dist[i][k] + dist[k][j];

                    if (cost_nou < dist[i][j]) {
                        /* Actualizam distanta daca este cazul*/
                        dist[i][j] = cost_nou;
                        next[i][j] = next[i][k];
                    }
                }
            }
        }
    }

    /* Ne intereseaza drumul Bucuresti Nord - Braila */
    int start = cauta_oras(orase, nr_orase, "Bucuresti Nord");
    int end = cauta_oras(orase, nr_orase, "Braila");

    /* Scriem drumul in fisier si distanta totala */
    FILE *output = fopen("ruta.txt", "w");
    afisare_drum(next, orase, start, end, output);
    fprintf(output, "Distanta totala: %d km\n", dist[start][end]);
    fclose(output);

    /* Calculam nodul cel mai departe de bucuresti si distanta */
    int distanta_maxima = 0;
    int statie = 0;
    for (int i = 0; i < nr_orase; i++) {
        if (dist[start][i] != INFINIT) {
            if (dist[start][i] > distanta_maxima) {
                /* Actualizam distanta daca e cazul */
                distanta_maxima = dist[start][i];
                statie = i;
            }
        }
    }

    /* Scriem in fisier output-ul si distanta totala */
    FILE *output_departe = fopen("departe.txt", "w");
    afisare_drum(next, orase, start, statie, output);
    fprintf(output, "Distanta totala: %d km\n", distanta_maxima);
    fclose(output);

    /* Calculam distanta maxima dintre oricare 2 statii */
    distanta_maxima = 0;
    int statie_start = 0;
    int statie_end = 0;
    for (int i = 0; i < nr_orase; i++) {
        for (int j = 0; j < nr_orase; j++) {
            if (i != j && dist[i][j] != INFINIT) {
                if (dist[i][j] > distanta_maxima) {
                    distanta_maxima = dist[i][j];
                    statie_start = i;
                    statie_end = j;
                }
            }
        }
    }
    /* Scriem in fisier output-ul si distanta totala */
    FILE *output_drum_lung = fopen("drum_lung.txt", "w");
    afisare_drum(next, orase, statie_start, statie_end, output_drum_lung);
    fprintf(output_drum_lung, "Distanta totala: %d km\n", distanta_maxima);
    fclose(output_drum_lung);

    /* Eliberam memoria */
    for (int i = 0; i < 1000; i++) {
        free(dist[i]);
        free(next[i]);
    }
    free(dist);
    free(next);
}

/* Functie recursiva pentru afisarea rutelor bucuresti-oradea */
void afiseaza_drum(int u, int d, int *vizitate, int *cale, int *index_cale, int **adiacenta, char **orase, int nr_orase, FILE *output) {

    /* Marcam nodul curent si il tinem minte in cale */
    vizitate[u] = 1;
    cale[*index_cale] = u;
    (*index_cale)++;

    if (*index_cale > 100) {  // conditie de oprire
        for (int i = 0; i < nr_orase; i++) {
            vizitate[i] = 1;
        }
        return;
    }

    if (u == d) {
        /* Afisam calea cand am ajuns la destinatie */
        for (int i = 0; i < *index_cale; i++) {
            fprintf(output, "%s", orase[cale[i]]);
            if (i !=  *index_cale - 1) {
                fprintf(output, ", ");
            }
        }
        fprintf(output, "\n");
    } else {
        for (int i = 0; i < nr_orase; i++) {
            if (adiacenta[u][i] == 1) {
                if (vizitate[i] == 0) {
                    /* Apel recursiv pentru a trece la urmatorul vecin */
                    afiseaza_drum(i, d, vizitate, cale, index_cale, adiacenta, orase, nr_orase, output);
                }
            }
        }
    }

    /* Uitam de elementul curent si il marcam ca fiind nevizitat */
    (*index_cale)--;
    vizitate[u] = 0;
}

void drumuri_bucuresti_oradea(int **adiacenta, int **distante, char **orase, int nr_orase) {
    /* Alocam vectorii pentru orasele vizitate si pentru calea curenta */
    int *vizitate = calloc(nr_orase, sizeof(int));
    int *cale = calloc(nr_orase, sizeof(int));

    int index_cale = 0;

    int start = cauta_oras(orase, nr_orase, "Bucuresti Nord");
    int end = cauta_oras(orase, nr_orase, "Oradea");

    /* Scriem output-ul */
    FILE *output = fopen("bucuresti_oradea.txt", "w");
    afiseaza_drum(start, end, vizitate, cale, &index_cale, adiacenta, orase, nr_orase, output);
    fclose(output);

    /* Eliberam memoria */
    free(vizitate);
    free(cale);
}

int main() {
    /* Buffere pentru citire*/
    char buffer[100];
    char buffer_traseu[100];

    /* Alocam matricea de adiacenta pentru graful neorientat */
    int **adiacenta = malloc(1000 * sizeof(int*));
    for (int i = 0; i < 1000; i++) {
        adiacenta[i] = calloc(1000, sizeof(int));
    }

    /* Alocam matricea de adiacenta pentru graful orientat */
    int **adiacenta_orientat = malloc(1000 * sizeof(int*));
    for (int i = 0; i < 1000; i++) {
        adiacenta_orientat[i] = calloc(1000, sizeof(int));
    }

    /* Alocam matricea de distante */
    int **distante = malloc(1000 * sizeof(int*));
    for (int i = 0; i < 1000; i++) {
        distante[i] = calloc(1000, sizeof(int));
    }

    /* Alocam vectorul de orase */
    char **orase = malloc(1000 * sizeof(char*));
    for (int i = 0; i < 1000; i++) {
        orase[i] = calloc(50, sizeof(char));
    }
    int nr_orase = 0;


    FILE *input = fopen("_all_files.txt", "r");
    while(fgets(buffer, 100, input)) {  // Pentru fiecare fisier (scris in _all_files.txt)
        strip(buffer);

        int index_oras = 0;  // Indexul orasului din fisier
        char oras_start[50];  // Buffer pentru numele statiei de la care pleaca trenul
        int index_start = -1;  // Initializam indexul statiei de la care pleaca trenul
        int distanta_anterioara = 0;  // Distanta de la statia initiala pana la statia anterioara

        FILE *input_traseu = fopen(buffer, "r");
        while(fgets(buffer_traseu, 100, input_traseu)) {  // Pentru fiecare oras din fisier
            strip(buffer_traseu);  // Eliminam \n
            char *distanta = strchr(buffer_traseu, ',');  // Salvam un pointer unde este virgula
            distanta[0] = '\0';  // Inlocuim virgula cu terminatorul de string
            distanta += 2;  // Adaugam 2 la pointer pentru a trece de virgula si spatiu

            /* Astfel, in buffer_traseu va ramane numele statiei, iar in distanta distanta */

            /* Cautam orasul curent */
            int oras_curent = cauta_oras(orase, nr_orase, buffer_traseu);
            if (oras_curent == -1) {
                /* Daca orasul nu exista, il salvam si actualizam numarul*/
                oras_curent = nr_orase;
                strcpy(orase[oras_curent], buffer_traseu);
                nr_orase++;
            }
            if (index_oras == 0) {
                /* Salvam numele orasului de start, indexul si initializam distanta anterioara cu 0 */
                strcpy(oras_start, buffer_traseu);
                index_start = oras_curent;
                distanta_anterioara = 0;
            } else {
                /* Actualizam matricele de adiacenta si matricea de distante */
                adiacenta[index_start][oras_curent] = 1;
                adiacenta[oras_curent][index_start] = 1;

                adiacenta_orientat[index_start][oras_curent] = 1;

                distante[index_start][oras_curent] = atoi(distanta) - distanta_anterioara;
                distante[oras_curent][index_start] = atoi(distanta) - distanta_anterioara;

                /*  In fisier avand distanta de la orasul initial, aici vom avea la graf
                    muchie intre statia precedenta si statia curenta, distanta anterioara scazandu-se */
                distanta_anterioara += distante[index_start][oras_curent];

                /* Actualizam statia curenta */
                index_start = oras_curent;
            }
            /* Actualizam numarul orasului */
            index_oras++;
        }
        /* Inchidem fisierul traseului */
        fclose(input_traseu);
    }
    /* Inchidem fisierul care contine denumirile tuturor fisierelor */
    fclose(input);

    /* Apelam functiile */
    statii_min_max(adiacenta, distante, orase, nr_orase);
    distanta_cale_ferata(adiacenta, distante, nr_orase);
    dist_braila_departe(adiacenta, distante, orase, nr_orase);
    drumuri_bucuresti_oradea(adiacenta, distante, orase, nr_orase);


    /* Eliberam memoria */
    for (int i = 0; i < 1000; i++) {
        free(adiacenta[i]);
        free(adiacenta_orientat[i]);
        free(distante[i]);
        free(orase[i]);
    }
    free(adiacenta);
    free(adiacenta_orientat);
    free(distante);
    free(orase);
    return 0;
}
