#include <stdio.h>
#include <stdlib.h>

#include "global_vars.h"
#include "types.h"
#include "world.h"

void readInput() {
    scanf("%d %d %d %d %d %d %d\n", &GEN_PROC_RABBITS,
          &GEN_PROC_FOXES, &GEN_FOOD_FOXES, &N_GEN, &R, &C, &N);

    oldWorld = buildWorld();
    newWorld = buildWorld();

    for (int i = 0; i < N; i++) {
        Type type;
        if (getchar() == 'F') {
            fseek(stdin, 3, SEEK_CUR);
            type = FOX;
        } else if (getchar() == 'A') {
            fseek(stdin, 5, SEEK_CUR);
            type = RABBIT;
        } else {
            fseek(stdin, 3, SEEK_CUR);
            type = ROCK;
        }

        int x, y;
        scanf("%d %d\n", &x, &y);

        if(type == ROCK) {
            newWorld[x][y].type = type;
        }

        oldWorld[x][y].type = type;

        oldWorld[x][y].currentGenFood = 0;
        oldWorld[x][y].currentGenProc = 0;
    }
}

void writeOutput() {
    N = 0;
    for (int i = 0; i < C; i++) {
        for (int j = 0; j < R; j++) {
            if (getType(oldWorld[i][j].type) != EMPTY) {
                N++;
            }
        }
    }

    printf("%d %d %d %d %d %d %d\n", GEN_PROC_RABBITS,
           GEN_PROC_FOXES, GEN_FOOD_FOXES, N_GEN - ACTUAL_GEN, R, C, N);

    for (int x = 0; x < R; x++) {
        for (int y = 0; y < C; y++) {
            Type t = getType(oldWorld[x][y].type);
            if (t != EMPTY) {
                printf("%s %d %d\n", getFullName(t), x, y);
            }
        }
    }

    destroyWorld(oldWorld);
    destroyWorld(newWorld);
}

void saveGeneration() {
    #pragma omp parallel for collapse(2)
    for (int x = 0; x < R; x++) {
        for (int y = 0; y < C; y++) {
            if(oldWorld[x][y].type != ROCK) {
                oldWorld[x][y].type = newWorld[x][y].type;
                oldWorld[x][y].currentGenFood = newWorld[x][y].currentGenFood;
                oldWorld[x][y].currentGenProc = newWorld[x][y].currentGenProc;

                newWorld[x][y].type = EMPTY;
                newWorld[x][y].currentGenFood = 0;
                newWorld[x][y].currentGenProc = 0;
            }
        }
    }
}

void decodePos(int p, int i, int j, int* x, int* y) {
    switch (p) {
        case 0:
            *x = i - 1;
            *y = j;
            break;

        case 1:
            *x = i;
            *y = j + 1;
            break;

        case 2:
            *x = i + 1;
            *y = j;
            break;

        case 3:
            *x = i;
            *y = j - 1;
            break;
        default: 
        printf(" p = %d i = %d j = %d \n",p,i,j);
        exit(1);
    }
}

int calculateDirection(int i, int j, int* p, int psum) {
    int moveTo = (ACTUAL_GEN + i + j) % psum;
    int pos;
    for (pos = 0; pos < 4; pos++) {
        if (p[pos] != 0) {
            if (moveTo == 0)
                break;
            moveTo--;
        }
    }
    return pos;
}

void writeState() {
    printf("Gen %d\n", ACTUAL_GEN);

    for (int i = 0; i < C + 2; i++)
        printf("-");
    printf("\n");

    for (int i = 0; i < R; i++) {
        printf("|");
        for (int j = 0; j < C; j++) {
            printf("%c", getMinName(getType(oldWorld[i][j].type)));
        }
        printf("|\n");
    }

    for (int i = 0; i < C + 2; i++)
        printf("-");
    printf("\n");
}

void writeStateAll() {
    printf("Generation %d\n", ACTUAL_GEN);

    for (int i = 0; i < C + 2; i++) printf("-");
    printf("   ");
    for (int i = 0; i < C + 2; i++) printf("-");
    printf(" ");
    for (int i = 0; i < C + 2; i++) printf("-");
    printf("\n");

    for (int i = 0; i < R; i++) {
        printf("|");
        for (int j = 0; j < C; j++) {
            printf("%c", getMinName(getType(oldWorld[i][j].type)));
        }
        printf("|   |");
        for (int j = 0; j < C; j++) {
            if(oldWorld[i][j].type == ROCK) {
                printf("*");
            } else if(oldWorld[i][j].type == EMPTY) {
                printf(" ");
            } else {
                printf("%d", oldWorld[i][j].currentGenProc);
            }
        }
        printf("| |");
        for (int j = 0; j < C; j++) {
            if(oldWorld[i][j].type == ROCK) {
                printf("*");
            } else if(oldWorld[i][j].type == EMPTY) {
                printf(" ");
            } else if(oldWorld[i][j].type == RABBIT){
                printf("R");
            } else {
                printf("%d", oldWorld[i][j].currentGenFood);
            }
        }
        printf("|\n");
    }

    for (int i = 0; i < C + 2; i++) printf("-");
    printf("   ");
    for (int i = 0; i < C + 2; i++) printf("-");
    printf(" ");
    for (int i = 0; i < C + 2; i++) printf("-");
    printf("\n\n");
}